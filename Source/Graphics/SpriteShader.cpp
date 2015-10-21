#include "SpriteShader.h"
#include "../Helper/GeneralHelper.h"
#include "Shaders/Compiled/SpriteVS.h"
#include "Shaders/Compiled/SpritePS.h"
#include "Shaders/Compiled/SpriteGS.h"

using namespace DirectX;

SpriteShader::SpriteShader()
	:m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_matrixBuffer(0),
	m_lightBuffer(0),
	m_samplerState(0)
{
}

SpriteShader::~SpriteShader()
{
}

bool SpriteShader::Initialize(ID3D11Device* device, HWND window, Material material)
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

void SpriteShader::Shutdown()
{
	ShutdownShader();
}

bool SpriteShader::Render(ID3D11DeviceContext* deviceContext,
	const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &proj, BasicLight lightData, Fog fog, XMFLOAT3 eyePos,
	ID3D11ShaderResourceView* texture, UINT vertexCountOffset)
{
	bool result;

	result = SetShaderParameters(deviceContext, world, view, proj, lightData, fog, eyePos, texture);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, vertexCountOffset);

	return true;
}

bool SpriteShader::InitializeShader(ID3D11Device* device, HWND window)
{
	D3D11_INPUT_ELEMENT_DESC inputLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC fogBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	HRESULT result;

	result = device->CreateVertexShader(SPRITE_VS, sizeof(SPRITE_VS), nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		MessageBox(0, L"Could not create vertex shader.", 0, MB_OK);
		return false;
	}

	result = device->CreateGeometryShader(SPRITE_GS, sizeof(SPRITE_GS), nullptr, &m_geometryShader);
	if (FAILED(result))
	{
		MessageBox(0, L"Could not create geometry shader.", 0, MB_OK);
		return false;
	}

	result = device->CreatePixelShader(SPRITE_PS, sizeof(SPRITE_PS), nullptr, &m_pixelShader);
	if (FAILED(result))
	{
		MessageBox(0, L"Could not create pixel shader.", 0, MB_OK);
		return false;
	}

	inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	inputLayout[1] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);

	result = device->CreateInputLayout(inputLayout, numElements, SPRITE_VS, sizeof(SPRITE_VS), &m_layout);
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

	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(FogBuffer);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&fogBufferDesc, nullptr, &m_fogBuffer);
	if (FAILED(result))
	{
		MessageBox(0, L"Could not create fog buffer.", 0, MB_OK);
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

void SpriteShader::ShutdownShader()
{
	ReleaseCOM(m_samplerState);
	ReleaseCOM(m_lightBuffer);
	ReleaseCOM(m_matrixBuffer);
	ReleaseCOM(m_fogBuffer);
	ReleaseCOM(m_layout);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
	ReleaseCOM(m_geometryShader);
}

bool SpriteShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &proj, BasicLight lightData, Fog fog, XMFLOAT3 eyePos, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* dataPtr;
	LightBuffer* dataPtr2;
	FogBuffer* dataPtr3;
	unsigned int bufferNumber;

	//-------------------------------------------------------------
	//-------Set Constant Buffers
	//-------------------------------------------------------------

	//----------------Map Matrix Buffer
	XMMATRIX worldCpy, viewCpy, projCpy;
	XMFLOAT4X4 worldInv, textCpy;

	worldCpy = XMMatrixTranspose(world);
	viewCpy = XMMatrixTranspose(view);
	projCpy = XMMatrixTranspose(projCpy);

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
	dataPtr->worldInvTranspose = worldInv;
	XMStoreFloat4x4(&dataPtr->texTransform, XMMatrixTranspose(textTransf));

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
	dataPtr2->dirLight = lightData.Directional;
	dataPtr2->pointLight = lightData.Point;
	dataPtr2->spotLight = lightData.Spot;
	dataPtr2->material = m_material;
	dataPtr2->eyePos = eyePos;

	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	//----------------Map Fog Buffer
	result = deviceContext->Map(m_fogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr3 = (FogBuffer*)mappedResource.pData;
	dataPtr3->fog = fog;

	deviceContext->Unmap(m_fogBuffer, 0);

	bufferNumber = 1;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_fogBuffer);

	//-------------------------------------------------------------
	//-------Set Shader Resource
	//-------------------------------------------------------------

	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void SpriteShader::RenderShader(ID3D11DeviceContext* deviceContext, UINT vertexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	deviceContext->Draw(1, vertexCount);
}