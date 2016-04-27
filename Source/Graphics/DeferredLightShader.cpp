#include "DeferredLightShader.h"
#include "Shaders\Compiled\DeferredLightVS.h"
#include "Shaders\Compiled\DeferredLightPS.h"
#include "../Helper/GeneralHelper.h"

using namespace DirectX;

DeferredLightShader::DeferredLightShader()
	:m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_samplerState(0),
	m_matrixBuffer(0),
	m_lightBuffer(0)
{
}

DeferredLightShader::~DeferredLightShader()
{
}

bool DeferredLightShader::Initialize(ID3D11Device * device, HWND hWnd)
{
	bool result;

	result = InitializeShader(device, hWnd);
	if (!result)
	{
		MessageBox(0, L"Cant initalize deferred light shader InitalizeShader func", L"Error", MB_OK);
		return false;
	}

	return true;
}

void DeferredLightShader::Shutdown()
{
	ShutdownShader();
}

bool DeferredLightShader::Render(ID3D11DeviceContext * deviceContext, offsetData offset, DirectX::XMMATRIX & world, DirectX::XMMATRIX & view, DirectX::XMMATRIX & projection, ID3D11ShaderResourceView * albedo, ID3D11ShaderResourceView * normal, DirectX::XMFLOAT3 lightDirection)
{
	bool result;

	result = setShaderParameters(deviceContext, world, view, projection, albedo, normal, lightDirection);
	if (!result)
	{
		MessageBox(0, L"Cant initalize set parameters for deferred light shader", L"Error", MB_OK);
		return false;
	}

	RenderShader(deviceContext, offset);

	return true;
}

bool DeferredLightShader::InitializeShader(ID3D11Device * device, HWND hWnd)
{
	HRESULT result;
	D3D11_INPUT_ELEMENT_DESC layout[2];
	UINT numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	result = device->CreateVertexShader(DLVS, sizeof(DLVS), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		//error msg here
		MessageBox(0, L"Cant create vertex shader", L"Error", MB_OK);
		return false;
	}

	result = device->CreatePixelShader(DLPS, sizeof(DLPS), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		//error msg here
		MessageBox(0, L"Cant create pixel shader", L"Error", MB_OK);
		return false;
	}

	//layout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	//layout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	layout[0].SemanticName = "POSITION";
	layout[0].SemanticIndex = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].AlignedByteOffset = 0;
	layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;

	layout[1].SemanticName = "TEXCOORD";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	numElements = sizeof(layout) / sizeof(layout[0]);

	result = device->CreateInputLayout(layout, numElements, DLVS, sizeof(DLVS), &m_layout);
	if (FAILED(result))
	{
		//error msg here
		MessageBox(0, L"Cant create input layout", L"Error", MB_OK);
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
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
		MessageBox(0, L"Cant create sampler state", L"Error", MB_OK);
		return false;
	}
	/*
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		MessageBox(0, L"Cant create matrix buffer", L"Error", MB_OK);
		return false;
	}
	*/
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		MessageBox(0, L"Cant create light buffer", L"Error", MB_OK);
		return false;
	}

	return true;
}

void DeferredLightShader::ShutdownShader()
{
	ReleaseCOM(m_lightBuffer);
	ReleaseCOM(m_matrixBuffer);
	ReleaseCOM(m_samplerState);
	ReleaseCOM(m_layout);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
}

bool DeferredLightShader::setShaderParameters(ID3D11DeviceContext * deviceContext, DirectX::XMMATRIX & world, DirectX::XMMATRIX & view, DirectX::XMMATRIX & projection, ID3D11ShaderResourceView * albedo, ID3D11ShaderResourceView * normal, DirectX::XMFLOAT3 lightDirection)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UINT bufferNumber;
	MatrixBufferType* dataPtr1;
	LightBufferType* dataPtr2;
	/*
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr1 = (MatrixBufferType*)mappedResource.pData;
	dataPtr1->world = world;
	dataPtr1->view = view;
	dataPtr1->projection = projection;

	deviceContext->Unmap(m_matrixBuffer, 0);
	
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	*/
	deviceContext->PSSetShaderResources(0, 1 ,&albedo);
	deviceContext->PSSetShaderResources(1, 1, &normal);

	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (LightBufferType*)mappedResource.pData;

	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void DeferredLightShader::RenderShader(ID3D11DeviceContext * deviceContext, offsetData offset)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	deviceContext->DrawIndexed(offset.indexCount, offset.indexOffset, offset.vertexOffset);
}
