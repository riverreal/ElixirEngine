#include "LightShader.h"
#include "../Helper/GeneralHelper.h"
#include "Shaders/Compiled/LightPixelShader.h"
#include "Shaders/Compiled/LightVertexShader.h"

using namespace DirectX;

LightShader::LightShader()
	:m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_matrixBuffer(0),
	m_lightBuffer(0)
{
}

LightShader::LightShader(const LightShader& other)
{
}

LightShader::~LightShader()
{
}

bool LightShader::Initialize(ID3D11Device* device, HWND window, Material material)
{
	bool result;

	result = InitializeShader(device, window);
	if (!result)
	{
		return false;
	}

	m_material = material;

	return true;
}

void LightShader::Shutdown()
{
	ShutdownShader();
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext,
	const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &proj, DirectionalLight dirLight, XMFLOAT3 eyePos,
	UINT indexCount, UINT indexOffset, UINT vertexOffset)
{
	bool result;

	result = SetShaderParameters(deviceContext, world, view, proj, dirLight, eyePos);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount, indexOffset, vertexOffset);

	return true;
}

bool LightShader::InitializeShader(ID3D11Device* device, HWND window)
{
	D3D11_INPUT_ELEMENT_DESC inputLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	HRESULT result;

	result = device->CreateVertexShader(LVS, sizeof(LVS), nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(LPS, sizeof(LPS), nullptr, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	inputLayout[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);

	result = device->CreateInputLayout(inputLayout, numElements, LVS, sizeof(LVS), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
	if (FAILED(result))
	{
		MessageBox(0, L"Could not create light buffer.", 0, MB_OK);
		return false;
	}

	return true;
}

void LightShader::ShutdownShader()
{
	ReleaseCOM(m_lightBuffer);
	ReleaseCOM(m_matrixBuffer);
	ReleaseCOM(m_layout);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &proj, DirectionalLight dirLight, XMFLOAT3 eyePos)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* dataPtr;
	LightBuffer* dataPtr2;
	unsigned int bufferNumber;

	//----------------Map Matrix Buffer
	XMMATRIX worldCpy, viewCpy, projCpy;
	XMFLOAT4X4 worldInv;

	worldCpy = XMMatrixTranspose(world);
	XMStoreFloat4x4(&worldInv, worldCpy);
	viewCpy = XMMatrixTranspose(view);
	projCpy = XMMatrixTranspose(proj);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBuffer*)mappedResource.pData;
	dataPtr->world = worldCpy;
	dataPtr->view = viewCpy;
	dataPtr->projection = projCpy;
	dataPtr->worldInvTranspose = worldInv;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//----------------Map Light Buffer
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (LightBuffer*)mappedResource.pData;
	dataPtr2->dirLight = dirLight;
	dataPtr2->eyePos = eyePos;
	dataPtr2->material = m_material;

	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, UINT indexCount, UINT indexOffset, UINT vertexOffset)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	deviceContext->DrawIndexed(indexCount, indexOffset, vertexOffset);
}