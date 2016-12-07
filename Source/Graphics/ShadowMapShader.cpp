#include "ShadowMapShader.h"
#include "../Helper/GeneralHelper.h"
#include "Shaders/Compiled/ShadowMapShader.h"

using namespace DirectX;

ShadowMapShader::ShadowMapShader()
	:m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_depthMatrixBuffer(0)
{
}

ShadowMapShader::~ShadowMapShader()
{
}

bool ShadowMapShader::Initialize(ID3D11Device * device)
{
	bool result;

	result = InitializeShader(device);
	if (!result)
	{
		RadixLog("Failed to initialize shader in ShadowMapShader");
		return result;
	}

	return true;
}

void ShadowMapShader::Render(ID3D11DeviceContext * deviceContext, Object * object, Light * light)
{
	RenderShader(deviceContext, object, light);
}

void ShadowMapShader::Shutdown()
{
	ReleaseCOM(m_depthMatrixBuffer);
	ReleaseCOM(m_layout);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
}

bool ShadowMapShader::InitializeShader(ID3D11Device * device)
{
	HRESULT result;
	D3D11_INPUT_ELEMENT_DESC layout[2];
	UINT numElem;
	D3D11_BUFFER_DESC matrixBufferDesc;

	result = device->CreateVertexShader(SMS_VS, sizeof(SMS_VS), nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		RadixLog("Failed to create vertex shader in ShadowMapShader");
		return false;
	}

	layout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	layout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	numElem = sizeof(layout) / sizeof(layout[0]);

	result = device->CreateInputLayout(layout, numElem, SMS_VS, sizeof(SMS_VS), &m_layout);
	if (FAILED(result))
	{
		RadixLog("Failed to create input layout in ShadowMapShader");
		return false;
	}

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(DepthMatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_depthMatrixBuffer);
	if (FAILED(result))
	{
		RadixLog("Failed to create buffer in ShadowMapShader");
		return false;
	}
	
	return true;
}

void ShadowMapShader::RenderShader(ID3D11DeviceContext * deviceContext, Object * object, Light * light)
{
	HRESULT result;
	XMMATRIX projMatrix;
	XMMATRIX viewMatrix;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMMATRIX worldCpy, viewCpy, projCpy, texTransCpy;
	DepthMatrixBuffer* dataPtr;

	//Calculate light's view and projection matrices
	light->BuildShadowTransform();
	//View----------------------------
	viewMatrix = light->GetLightViewMatrix();

	//Proj----------------------------
	projMatrix = light->GetLightProjMatrix();


	//Set Constant buffers-------------------------------------------------
	worldCpy = XMMatrixTranspose(object->GetWorldMatrix());
	viewCpy = XMMatrixTranspose(viewMatrix);
	projCpy = XMMatrixTranspose(projMatrix);
	texTransCpy = XMMatrixTranspose(object->GetTexTransform());

	result = deviceContext->Map(m_depthMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		RadixLog("Error mapping the depth matrix buffer");
	}

	dataPtr = (DepthMatrixBuffer*)mappedResource.pData;
	dataPtr->world = worldCpy;
	dataPtr->view = viewCpy;
	dataPtr->projection = projCpy;
	dataPtr->texTrans = texTransCpy;

	deviceContext->Unmap(m_depthMatrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &m_depthMatrixBuffer);

	//Draw
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);

	deviceContext->DrawIndexed(object->GetOffset().indexCount, object->GetOffset().indexOffset, object->GetOffset().vertexOffset);
}
