#include "Post-processingShader.h"
#include "../Helper/GeneralHelper.h"
#include "Shaders/Compiled/PostProcessingVS.h"
#include "Shaders/Compiled/PostProcessingPS.h"

PostProcessShader::PostProcessShader()
	:m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_samplerState(0)
{
}

PostProcessShader::~PostProcessShader()
{
}

bool PostProcessShader::Initialize(ID3D11Device * device, HWND hWnd)
{
	bool result;

	result = InitializeShader(device, hWnd);
	if (!result)
	{
		RadixLog("Couldn't initialize PostProcessShader");
		return false;
	}

	return true;
}

void PostProcessShader::Shutdown()
{
	ShutdownShader();
}

bool PostProcessShader::Render(ID3D11DeviceContext * deviceContext, offsetData offset, ID3D11ShaderResourceView * unProcessedImage)
{
	bool result;

	result = SetShaderParameters(deviceContext, unProcessedImage);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, offset);

	return true;
}

bool PostProcessShader::InitializeShader(ID3D11Device * device, HWND hWnd)
{
	HRESULT result;
	D3D11_INPUT_ELEMENT_DESC layout;
	UINT numElements;
	D3D11_SAMPLER_DESC samplerDesc;

	result = device->CreateVertexShader(PP_VS, sizeof(PP_VS), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		RadixLog("Could not create vertex shader");
		return false;
	}

	result = device->CreatePixelShader(PP_PS, sizeof(PP_PS), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		RadixLog("Could not create pixel shader");
		return false;
	}

	layout.SemanticName = "POSITION";
	layout.SemanticIndex = 0;
	layout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout.InputSlot = 0;
	layout.AlignedByteOffset = 0;
	layout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout.InstanceDataStepRate = 0;

	numElements = 1;

	result = device->CreateInputLayout(&layout, numElements, PP_VS, sizeof(PP_VS), &m_layout);
	if (FAILED(result))
	{
		RadixLog("Could not create input layout");
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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
		RadixLog("Could not create sampler state");
		return false;
	}

	return true;
}

void PostProcessShader::ShutdownShader()
{
	ReleaseCOM(m_samplerState);
	ReleaseCOM(m_layout);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
	
}

bool PostProcessShader::SetShaderParameters(ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * unProcessedImage)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	deviceContext->PSSetShaderResources(0, 1, &unProcessedImage);

	return true;
}

void PostProcessShader::RenderShader(ID3D11DeviceContext * deviceContext, offsetData offset)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	
	//deviceContext->DrawIndexed(offset.indexCount, offset.indexOffset, offset.vertexOffset);
	deviceContext->Draw(3, 0);
}
