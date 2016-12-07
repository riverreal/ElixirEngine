#include "DeferredLightShader.h"
#include "Shaders\Compiled\DeferredLightVS.h"
#include "Shaders\Compiled\DeferredLightPS.h"
#include "../Helper/GeneralHelper.h"
#include <string>

using namespace DirectX;

DeferredLightShader::DeferredLightShader()
	:m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_samplerState(0),
	m_shadowSampler(0),
	m_fogBuffer(0),
	m_cbPerObject(0),
	m_lightBuffer(0),
	m_shadowMapBuffer(0)
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

bool DeferredLightShader::Render(ID3D11DeviceContext* deviceContext, offsetData offset,
	ID3D11ShaderResourceView* envMap, ID3D11ShaderResourceView* irradiance, ID3D11ShaderResourceView* albedo, ID3D11ShaderResourceView* normal, 
	ID3D11ShaderResourceView* matProp, ID3D11ShaderResourceView* position, ID3D11ShaderResourceView* shadowMap, Light* lighting, Camera* camera, Fog fog)
{
	bool result;

	result = setShaderParameters(deviceContext, albedo, normal, matProp, position, irradiance, envMap, shadowMap, lighting, camera, fog);
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
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC fogBufferDesc;

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

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
		MessageBox(0, L"Cant create sampler state", L"Error", MB_OK);
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	result = device->CreateSamplerState(&samplerDesc, &m_shadowSampler);
	if (FAILED(result))
	{
		MessageBox(0, L"Cant create shadow sampler state", L"Error", MB_OK);
		return false;
	}
	
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
	if (FAILED(result))
	{
		MessageBox(0, L"Cant create light buffer", L"Error", MB_OK);
		return false;
	}

	lightBufferDesc.ByteWidth = sizeof(cbPerObject);
	result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_cbPerObject);
	if (FAILED(result))
	{
		MessageBox(0, L"Cant create cdPerObject buffer", L"Error", MB_OK);
		return false;
	}

	lightBufferDesc.ByteWidth = sizeof(ShadowMapBuffer);
	result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_shadowMapBuffer);
	if (FAILED(result))
	{
		MessageBox(0, L"Cant create ShadowMapBuffer", L"Error", MB_OK);
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
		MessageBox(0, L"Cant create fog buffer", L"Error", MB_OK);
		return false;
	}

	

	return true;
}

void DeferredLightShader::ShutdownShader()
{
	ReleaseCOM(m_shadowMapBuffer);
	ReleaseCOM(m_fogBuffer);
	ReleaseCOM(m_cbPerObject);
	ReleaseCOM(m_lightBuffer);
	ReleaseCOM(m_shadowSampler);
	ReleaseCOM(m_samplerState);
	ReleaseCOM(m_layout);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
}

bool DeferredLightShader::setShaderParameters(ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * albedo, ID3D11ShaderResourceView * normal, ID3D11ShaderResourceView * matProp, ID3D11ShaderResourceView * position, 
	ID3D11ShaderResourceView * irradiance, ID3D11ShaderResourceView * envMap, ID3D11ShaderResourceView* shadowMap, Light* lighting, Camera* camera, Fog fog)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	UINT bufferNumber;
	LightBufferType* dataPtr1 = new LightBufferType;
	cbPerObject* dataPtrPO;
	FogBuffer* dataPtr2;
	ShadowMapBuffer* dataPtr0;

	//Gbuffer Textures---------------------------------------------------------------------------
	deviceContext->PSSetShaderResources(0, 1 ,&albedo);
	deviceContext->PSSetShaderResources(1, 1, &normal);
	deviceContext->PSSetShaderResources(2, 1, &matProp);
	deviceContext->PSSetShaderResources(3, 1, &position);

	//IBL Textures-------------------------------------------------------------------------------
	deviceContext->PSSetShaderResources(4, 1, &irradiance);
	deviceContext->PSSetShaderResources(5, 1, &envMap);

	//Shadow Map Texture-------------------------------------------------------------------------
	deviceContext->PSSetShaderResources(6, 1, &shadowMap);


	//ShadowMap Buffer---------------------------------------------------------------------------
	XMMATRIX lightView, lightProj, viewMatrix;
	lighting->BuildShadowTransform();
	lightView = XMMatrixTranspose(lighting->GetLightViewMatrix());
	lightProj = XMMatrixTranspose(lighting->GetLightProjMatrix());
	viewMatrix = XMMatrixTranspose(camera->GetViewMatrix());

	result = deviceContext->Map(m_shadowMapBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		RadixLog("Failed at shadow map buffer update");
		return false;
	}

	dataPtr0 = (ShadowMapBuffer*)mappedResource.pData;

	dataPtr0->lightProjMatrix = lightProj;
	dataPtr0->lightViewMatrix = lightView;
	dataPtr0->viewMatrix = viewMatrix;

	deviceContext->Unmap(m_shadowMapBuffer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &m_shadowMapBuffer);

	//Light Buffer-------------------------------------------------------------------------------
	
	PBRDirectionalLight dirL = lighting->GetDirectionalLight();
	auto numPoint = lighting->GetPointLightCount();
	auto numSpot = lighting->GetSpotLightCount();

	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	
	//dataPtr1 = (LightBufferType*)mappedResource.pData;
	
	dataPtr1->dirLight = dirL;
	dataPtr1->numPointLights = numPoint;
	dataPtr1->numSpotLights = numSpot;
	for (U32 i = 0; i < lighting->GetPointLightCount(); ++i)
	{
		dataPtr1->pointLight[i] = lighting->GetPointLight(i);
	}
	
	for (U32 i = 0; i < lighting->GetSpotLightCount(); ++i)
	{
		dataPtr1->spotLight[i] = lighting->GetSpotLight(i);
	}
	
	memcpy(mappedResource.pData, dataPtr1, sizeof(LightBufferType));

	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 1;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	SafeRelease(dataPtr1);

	//cbPerObject Buffer-----------------------------------------------------------------------

	result = deviceContext->Map(m_cbPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtrPO = (cbPerObject*)mappedResource.pData;

	dataPtrPO->eyePos = camera->GetPosition();

	deviceContext->Unmap(m_cbPerObject, 0);
	bufferNumber = 2;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_cbPerObject);

	//Fog Buffer-------------------------------------------------------------------------------
	result = deviceContext->Map(m_fogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (FogBuffer*)mappedResource.pData;
	dataPtr2->fog = fog;

	deviceContext->Unmap(m_fogBuffer, 0);
	bufferNumber = 3;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_fogBuffer);

	return true;
}

void DeferredLightShader::RenderShader(ID3D11DeviceContext * deviceContext, offsetData offset)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	deviceContext->PSSetSamplers(1, 1, &m_shadowSampler);

	deviceContext->DrawIndexed(offset.indexCount, offset.indexOffset, offset.vertexOffset);
}
