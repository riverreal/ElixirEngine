#include "SkyDome.h"
#include "../Helper/GeneralHelper.h"
#include "Shaders/Compiled/SkyVS.h"
#include "Shaders/Compiled/SkyPS.h"
#include "TextureManager.h"

using namespace DirectX;

SkyDome::SkyDome()
	:m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_matrixBuffer(0),
	m_samplerState(0)
{
}

SkyDome::~SkyDome()
{
}

bool SkyDome::Initialize(ID3D11Device* device, HWND window)
{
	bool result;

	result = InitializeShader(device, window);
	if (!result)
	{
		MessageBox(0, L"Couldnt initialize shaders", 0, 0);
		return false;
	}

	return true;
}

void SkyDome::Shutdown()
{
	ShutdownShader();
}

bool SkyDome::Render(ID3D11DeviceContext* deviceContext, Object* object, Camera camera, const XMMATRIX &proj, BasicLight lightData, Fog fog)
{
	bool result;

	XMMATRIX world = object->GetWorldMatrix();
	XMMATRIX view = camera.GetViewMatrix();
	XMFLOAT3 eyePos = camera.GetPosition();
	ID3D11ShaderResourceView* texture = object->GetTexture();
	XMMATRIX textTransf = object->GetTexTransform();
	Material material = object->GetMaterial();
	offsetData offset = object->GetOffset();

	result = SetShaderParameters(deviceContext, world, view, proj, lightData, fog, eyePos, texture, textTransf, material);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, offset);

	return true;
}

bool SkyDome::InitializeShader(ID3D11Device* device, HWND window)
{
	D3D11_INPUT_ELEMENT_DESC inputLayout[1];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	HRESULT result;

	result = device->CreateVertexShader(SKY_VS, sizeof(SKY_VS), nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		MessageBox(0, L"Could not create vertex shader.", 0, MB_OK);
		return false;
	}

	result = device->CreatePixelShader(SKY_PS, sizeof(SKY_PS), nullptr, &m_pixelShader);
	if (FAILED(result))
	{
		MessageBox(0, L"Could not create pixel shader.", 0, MB_OK);
		return false;
	}

	inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);

	result = device->CreateInputLayout(inputLayout, numElements, SKY_VS, sizeof(SKY_VS), &m_layout);
	if (FAILED(result))
	{
		MessageBox(0, L"Could not create input layout.", 0, MB_OK);
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
		MessageBox(0, L"Could not create matrix buffer.", 0, MB_OK);
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
	if (FAILED(result))
	{
		MessageBox(0, L"Can't create sampler state.", L"Error", MB_OK);
		return false;
	}


	return true;
}

void SkyDome::ShutdownShader()
{
	ReleaseCOM(m_samplerState);
	ReleaseCOM(m_matrixBuffer);
	ReleaseCOM(m_layout);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
}

bool SkyDome::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &proj, BasicLight lightData,
	Fog fog, XMFLOAT3 eyePos, ID3D11ShaderResourceView* texture, const XMMATRIX &textTransf, Material material)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* dataPtr;
	unsigned int bufferNumber;

	//-------------------------------------------------------------
	//-------Set Constant Buffers
	//-------------------------------------------------------------

	//----------------Map Matrix Buffer
	XMMATRIX worldCpy, viewCpy, projCpy;
	XMFLOAT4X4 worldInv, textCpy;

	worldCpy = XMMatrixTranspose(world);
	viewCpy = XMMatrixTranspose(view);
	projCpy = XMMatrixTranspose(proj);

	XMMATRIX A = worldCpy;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX inv = XMMatrixTranspose(XMMatrixInverse(&det, A));
	XMStoreFloat4x4(&worldInv, inv);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBuffer*)mappedResource.pData;
	dataPtr->world = worldCpy;
	dataPtr->view = viewCpy;
	dataPtr->projection = projCpy;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//-------------------------------------------------------------
	//-------Set Shader Resource
	//-------------------------------------------------------------

	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void SkyDome::RenderShader(ID3D11DeviceContext* deviceContext, offsetData offset)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	deviceContext->DrawIndexed(offset.indexCount, offset.indexOffset, offset.vertexOffset);
}