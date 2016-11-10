#include "Post-processingShader.h"
#include "../Helper/GeneralHelper.h"
#include "Shaders/Compiled/PostProcessingVS.h"
#include "Shaders/Compiled/PostProcessingPS.h"
#include "Shaders/Compiled/AdaptedLum.h"
#include "Shaders/Compiled/BrightPassFilter.h"
#include "Shaders/Compiled/TwoPassBlur.h"
#include <string>

PostProcessShader::PostProcessShader()
	:m_vertexShader(0),
	m_pixelShader(0),
	m_adaptLumShader(0),
	m_brightPassShader(0),
	m_blurShader(0),
	m_layout(0),
	m_samplerState(0),
	m_samplerStateNoWrap(0),
	m_firstFrame(true),
	m_oldSRVSwap(true),
	m_avrgIllumSRV(NULL)
{
	m_oldAdaptedIllumSRV[0] = 0;
	m_oldAdaptedIllumSRV[1] = 0;
}

PostProcessShader::~PostProcessShader()
{
}

bool PostProcessShader::Initialize(ID3D11Device * device, HWND hWnd, int textureWidth, int textureHeight, float screenDepth, float screenNear, D3D11_VIEWPORT defaultVP)
{
	bool result;

	

	result = InitializePostPro(device, textureWidth, textureHeight, screenDepth, screenNear);
	if (!result)
	{
		RadixLog("Could't initialize PostProcess");
		return false;
	}

	result = InitializeAvrgIll(device, textureWidth, textureHeight, screenDepth, screenNear);
	if (!result)
	{
		RadixLog("Could't initialize PostProcess Avrg Ill");
		return false;
	}

	result = InitializeShader(device, hWnd);
	if (!result)
	{
		RadixLog("Couldn't initialize PostProcessShader");
		return false;
	}

	m_defaultVP = defaultVP;

	return true;
}

void PostProcessShader::Shutdown()
{
	ShutdownShader();
}

bool PostProcessShader::Render(ID3D11DeviceContext * deviceContext, offsetData offset, float dt)
{
	bool result;

	//this stores the default render target (back buffer)
	ID3D11RenderTargetView* defRT;
	ID3D11DepthStencilView* defDSV;

	//Get the current render target (default that renders to backbuffer)
	deviceContext->OMGetRenderTargets(1, &defRT, &defDSV);
	
	//Get the original image to downsample until 1x1
	RenderAverageIllum(deviceContext);
	
	//Get the adapted luminance with proper old/new lum control
	RenderAdaptedLum(deviceContext, dt);

	RenderBloom(deviceContext);

	//Render to back buffer
	deviceContext->OMSetRenderTargets(1, &defRT, defDSV);
	float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	deviceContext->ClearRenderTargetView(defRT, color);
	deviceContext->ClearDepthStencilView(defDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_viewport.Width = (float) m_defaultVP.Width + 0.1f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Height = (float) m_defaultVP.Height + 0.1f;
	deviceContext->RSSetViewports(1, &m_viewport);

	//Post Process original image.
	RenderShader(deviceContext, offset);

	m_oldSRVSwap = !m_oldSRVSwap;

	return true;
}

void PostProcessShader::SetPostPRenderTarget(ID3D11DeviceContext * deviceContext)
{
	deviceContext->RSSetViewports(1, &m_defaultVP);
	deviceContext->OMSetRenderTargets(1, &m_auxRTV[0], NULL);
}

void PostProcessShader::ClearPostPRenderTarget(ID3D11DeviceContext * deviceContext)
{
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	deviceContext->ClearRenderTargetView(m_auxRTV[0], color);
}

bool PostProcessShader::InitializeShader(ID3D11Device * device, HWND hWnd)
{
	HRESULT result;
	D3D11_INPUT_ELEMENT_DESC layout;
	UINT numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC ppParamDesc;

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

	result = device->CreatePixelShader(AL_PS, sizeof(AL_PS), NULL, &m_adaptLumShader);
	if (FAILED(result))
	{
		RadixLog("Could not create adapt lum shader");
		return false;
	}

	result = device->CreatePixelShader(BPF_PS, sizeof(BPF_PS), NULL, &m_brightPassShader);
	if (FAILED(result))
	{
		RadixLog("Could not create Bright Pass Shader");
		return false;
	}

	result = device->CreatePixelShader(TPB_PS, sizeof(TPB_PS), NULL, &m_blurShader);
	if (FAILED(result))
	{
		RadixLog("Could not create Two Pass Blur Shader");
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

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = device->CreateSamplerState(&samplerDesc, &m_samplerStateNoWrap);
	if (FAILED(result))
	{
		RadixLog("Could not create sampler state");
		return false;
	}


	ppParamDesc.Usage = D3D11_USAGE_DYNAMIC;
	ppParamDesc.ByteWidth = sizeof(PostProcessParam);
	ppParamDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ppParamDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ppParamDesc.MiscFlags = 0;
	ppParamDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&ppParamDesc, nullptr, &m_postPParamBufer);
	if (FAILED(result))
	{
		return false;
	}

	ppParamDesc.ByteWidth = sizeof(AdaptedBuffer);
	result = device->CreateBuffer(&ppParamDesc, nullptr, &m_adaptedLumBuffer);
	if (FAILED(result))
	{
		return false;
	}

	ppParamDesc.ByteWidth = sizeof(blurBuffer);
	result = device->CreateBuffer(&ppParamDesc, nullptr, &m_blurBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool PostProcessShader::InitializeAvrgIll(ID3D11Device * device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ZeroMemory(&texDesc, sizeof(texDesc));
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	//m_avrgIllumSRVs.push_back(m_auxSRV[0]);

	//Initial numOfRT = 1 because we halfReso to get the first nearest pow of 2
	int numOfRT = 1;
	int halfResoX = GetNearestPow2(textureWidth);
	int halfResoY = GetNearestPow2(textureHeight);

	m_viewportX.push_back(halfResoX);
	m_viewportY.push_back(halfResoY);
	m_originalX = textureWidth;
	m_originalY = textureHeight;

	//Get the number of render targets needed to get 1x1 render target
	/*
	do
	{
		halfResoX /= 2;
		halfResoY /= 2;
		m_viewportX.push_back(halfResoX);
		m_viewportY.push_back(halfResoY);
		numOfRT++;
	} while (halfResoX != 1 && halfResoY != 1);
	*/
	while (halfResoX > 1)
	{
		halfResoX /= 2;
		if(halfResoY > 1)
			halfResoY /= 2;
		
		m_viewportX.push_back(halfResoX);
		m_viewportY.push_back(halfResoY);
		numOfRT++;
	}

	halfResoX = GetNearestPow2(textureWidth);
	halfResoY = GetNearestPow2(textureHeight);

	texDesc.Width = halfResoX;
	texDesc.Height = halfResoY;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	//Loop to initialize buffer, RTV and SRV
	for (U32 i = 0; i < numOfRT; ++i)
	{
		texDesc.Width = m_viewportX[i];
		texDesc.Height = m_viewportY[i];

		ID3D11Texture2D* buffer;
		ID3D11RenderTargetView* RTV;
		ID3D11ShaderResourceView* SRV;

		result = device->CreateTexture2D(&texDesc, NULL, &buffer);
		if (FAILED(result))
		{
			RadixLog("Failed to create avg illum buffer.");
			return false;
		}
		m_avrgIllumBuffers.push_back(buffer);

		result = device->CreateRenderTargetView(buffer, &rtvDesc, &RTV);
		if (FAILED(result))
		{
			RadixLog("Failed to create avg illum rtv.");
			return false;
		}
		m_avrgIllumRTV.push_back(RTV);

		result = device->CreateShaderResourceView(buffer, &srvDesc, &SRV);
		if (FAILED(result))
		{
			RadixLog("Failed to create avg illum srv.");
			return false;
		}
		m_avrgIllumSRVs.push_back(SRV);
	}

	m_viewport.Width = (float)m_viewportX[0];
	m_viewport.Height = (float)m_viewportY[0];
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	return true;
}

bool PostProcessShader::InitializePostPro(ID3D11Device * device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ZeroMemory(&texDesc, sizeof(texDesc));
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	texDesc.Width = textureWidth;
	texDesc.Height = textureHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	//loop through auxiliar buffers
	for (U32 i = 0; i < sizeof(m_auxBuffers)/sizeof(m_auxBuffers[0]); ++i)
	{
		//called once
		if (i == 1)
		{
			//half resolution the rest of the aux buffers except the first one (0).
			texDesc.Width = static_cast<float>(floor(textureWidth / 2));
			texDesc.Height = static_cast<float>(floor(textureHeight / 2));
		}

		result = device->CreateTexture2D(&texDesc, NULL, &m_auxBuffers[i]);
		if (FAILED(result))
		{
			return false;
		}

		result = device->CreateRenderTargetView(m_auxBuffers[i], &rtvDesc, &m_auxRTV[i]);
		if (FAILED(result))
		{
			return false;
		}

		result = device->CreateShaderResourceView(m_auxBuffers[i], &srvDesc, &m_auxSRV[i]);
		if (FAILED(result))
		{
			return false;
		}
	}

	texDesc.Width = 1;
	texDesc.Height = 1;

	result = device->CreateTexture2D(&texDesc, NULL, &m_oldAdaptedIllumBuffer[1]);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateRenderTargetView(m_oldAdaptedIllumBuffer[1], &rtvDesc, &m_oldAdaptedIllumRTV[1]);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateShaderResourceView(m_oldAdaptedIllumBuffer[1], &srvDesc, &m_oldAdaptedIllumSRV[1]);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateTexture2D(&texDesc, NULL, &m_oldAdaptedIllumBuffer[0]);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateRenderTargetView(m_oldAdaptedIllumBuffer[0], &rtvDesc, &m_oldAdaptedIllumRTV[0]);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateShaderResourceView(m_oldAdaptedIllumBuffer[0], &srvDesc, &m_oldAdaptedIllumSRV[0]);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void PostProcessShader::ShutdownShader()
{
	ReleaseCOM(m_samplerStateNoWrap);
	ReleaseCOM(m_samplerState);
	ReleaseCOM(m_layout);
	ReleaseCOM(m_blurShader);
	ReleaseCOM(m_brightPassShader);
	ReleaseCOM(m_adaptLumShader);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);

	for (auto &item : m_avrgIllumBuffers)
	{
		ReleaseCOM(item);
	}

	for (auto &item : m_avrgIllumRTV)
	{
		ReleaseCOM(item);
	}

	for (auto &item : m_avrgIllumSRVs)
	{
		ReleaseCOM(item);
	}

	ReleaseCOM(m_avrgIllumSRV);
	ReleaseCOM(m_oldAdaptedIllumBuffer[1]);
	ReleaseCOM(m_oldAdaptedIllumRTV[1]);
	ReleaseCOM(m_oldAdaptedIllumSRV[1]);
	ReleaseCOM(m_oldAdaptedIllumBuffer[0]);
	ReleaseCOM(m_oldAdaptedIllumRTV[0]);
	ReleaseCOM(m_oldAdaptedIllumSRV[0]);

	for (U32 i = 0; i < sizeof(m_auxBuffers)/sizeof(m_auxBuffers[0]); ++i)
	{
		ReleaseCOM(m_auxBuffers[i]);
		ReleaseCOM(m_auxRTV[i]);
		ReleaseCOM(m_auxSRV[i]);
	}
}

bool PostProcessShader::SetShaderParameters(ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * unProcessedImage, U32 enabled)
{	
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	PostProcessParam* dataPtr1 = new PostProcessParam;

	deviceContext->PSSetShaderResources(0, 1, &unProcessedImage);
	deviceContext->PSSetShaderResources(1, 1, &m_oldAdaptedIllumSRV[m_oldSRVSwap]);
	deviceContext->PSSetShaderResources(2, 1, &m_auxSRV[3]);

	result = deviceContext->Map(m_postPParamBufer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Set constant buffers
	dataPtr1->enabled = enabled;

	memcpy(mappedResource.pData, dataPtr1, sizeof(PostProcessParam));

	deviceContext->Unmap(m_postPParamBufer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &m_postPParamBufer);

	SafeRelease(dataPtr1);

	return true;
}

bool PostProcessShader::SetAdaptedShaderParam(ID3D11DeviceContext * deviceContext, float dt, U32 firstPass, ID3D11ShaderResourceView* source)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	AdaptedBuffer* dataPtr1 = new AdaptedBuffer;

	deviceContext->PSSetShaderResources(0, 1, &m_avrgIllumSRV);
	deviceContext->PSSetShaderResources(1, 1, &m_oldAdaptedIllumSRV[m_oldSRVSwap]);

	if (firstPass)
	{
		deviceContext->PSSetShaderResources(0, 1, &source);
	}

	result = deviceContext->Map(m_adaptedLumBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Set constant buffers
	dataPtr1->dt = dt;
	dataPtr1->firstPass = firstPass;

	memcpy(mappedResource.pData, dataPtr1, sizeof(AdaptedBuffer));

	deviceContext->Unmap(m_adaptedLumBuffer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &m_adaptedLumBuffer);

	SafeRelease(dataPtr1);

	return true;
}

bool PostProcessShader::SetBlurShaderParam(ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * imgToBlur, float textureWidth, float textureHeight, bool vertical)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	blurBuffer* dataPtr1 = new blurBuffer;

	deviceContext->PSSetShaderResources(0, 1, &imgToBlur);

	result = deviceContext->Map(m_blurBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Set constant buffers
	dataPtr1->passNumber = (float)vertical;
	dataPtr1->width = textureWidth;
	dataPtr1->height = textureHeight;

	memcpy(mappedResource.pData, dataPtr1, sizeof(AdaptedBuffer));

	deviceContext->Unmap(m_blurBuffer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &m_blurBuffer);

	SafeRelease(dataPtr1);

	return true;
}

void PostProcessShader::RenderShader(ID3D11DeviceContext * deviceContext, offsetData offset)
{
	SetShaderParameters(deviceContext, m_auxSRV[0], true);

	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerStateNoWrap);
	
	deviceContext->Draw(3, 0);
}

void PostProcessShader::RenderAverageIllum(ID3D11DeviceContext * deviceContext)
{
	deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	deviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Get halved image
	m_viewport.Width = (float)m_viewportX[0];
	m_viewport.Height = (float)m_viewportY[0];
	deviceContext->RSSetViewports(1, &m_viewport);
	deviceContext->OMSetRenderTargets(1, &m_avrgIllumRTV[0], NULL);
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	deviceContext->ClearRenderTargetView(m_avrgIllumRTV[0], color);

	SetShaderParameters(deviceContext, m_auxSRV[0], false);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	deviceContext->Draw(3, 0);
	
	//Get grayscale---------------------------------------------
	m_viewport.Width = (float)m_viewportX[1];
	m_viewport.Height = (float)m_viewportY[1];
	deviceContext->OMSetRenderTargets(1, &m_avrgIllumRTV[1], NULL);
	deviceContext->ClearRenderTargetView(m_avrgIllumRTV[1], color);
	deviceContext->RSSetViewports(1, &m_viewport);
	
	SetAdaptedShaderParam(deviceContext, 0.0, true, m_avrgIllumSRVs[0]);

	deviceContext->IASetInputLayout(NULL);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_adaptLumShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	deviceContext->Draw(3, 0);

	//Down sample until 1x1---------------------------------------------

	for (int i = 2; i < m_avrgIllumRTV.size(); ++i)
	{
		m_viewport.Width = (float)m_viewportX[i];
		m_viewport.Height = (float)m_viewportY[i];
		deviceContext->OMSetRenderTargets(1, &m_avrgIllumRTV[i], NULL);
		deviceContext->ClearRenderTargetView(m_avrgIllumRTV[i], color);
		deviceContext->RSSetViewports(1, &m_viewport);

		SetShaderParameters(deviceContext, m_avrgIllumSRVs[i - 1], false);

		deviceContext->IASetInputLayout(NULL);
		deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
		deviceContext->PSSetShader(m_adaptLumShader, nullptr, 0);
		deviceContext->PSSetSamplers(0, 1, &m_samplerState);

		deviceContext->Draw(3, 0);
	}

	//the final 1x1 average illuminance texture
	m_avrgIllumSRV = m_avrgIllumSRVs[m_avrgIllumSRVs.size() - 1];

	if (m_firstFrame)
	{
		//m_oldAdaptedIllumSRV[m_oldSRVSwap] = m_avrgIllumSRV;
		//memcpy(m_oldAdaptedIllumBuffer[m_oldSRVSwap], m_avrgIllumBuffers[m_avrgIllumBuffers.size() - 1], sizeof(m_avrgIllumBuffers[m_avrgIllumBuffers.size() - 1]));
		m_firstFrame = false;
	}
}

void PostProcessShader::RenderAdaptedLum(ID3D11DeviceContext * deviceContext, float dt)
{
	//Set render target
	deviceContext->OMSetRenderTargets(1, &m_oldAdaptedIllumRTV[!m_oldSRVSwap], NULL);
	m_viewport.Width = 1.0;
	m_viewport.Height = 1.0;
	deviceContext->RSSetViewports(1, &m_viewport);

	//Set shader resources
	SetAdaptedShaderParam(deviceContext, dt, false);

	//render
	deviceContext->IASetInputLayout(NULL);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_adaptLumShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	deviceContext->Draw(3, 0);

	/*
	//swap old lum after draw
	auto tmp = m_oldAdaptedIllumSRV[0];
	//[0] stores the result
	m_oldAdaptedIllumSRV[0] = m_oldAdaptedIllumSRV[1];
	m_oldAdaptedIllumSRV[1] = tmp;
	*/
}

void PostProcessShader::RenderBloom(ID3D11DeviceContext * deviceContext)
{
	//Render target set up:
	//ping pong between aux1 and 2
	//Aux0 ---> Aux1 ---> Aux2 ---> Aux1
	//HDR       Half     BlurX     BlurY

	deviceContext->OMSetRenderTargets(1, &m_auxRTV[1], NULL);
	m_viewport.Width = (float)floor(m_originalX / 2);
	m_viewport.Height = (float)floor(m_originalY / 2);
	deviceContext->RSSetViewports(1, &m_viewport);


	//Down sample and brigth pass filter
	deviceContext->PSSetShaderResources(0, 1, &m_auxSRV[0]);
	deviceContext->PSSetShaderResources(1, 1, &m_avrgIllumSRV);

	deviceContext->IASetInputLayout(NULL);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_brightPassShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerStateNoWrap);
	deviceContext->Draw(3, 0);

	//Horizontal blur
	deviceContext->OMSetRenderTargets(1, &m_auxRTV[2], NULL);

	bool vertical = false;
	SetBlurShaderParam(deviceContext, m_auxSRV[1], m_viewport.Width, m_viewport.Height, vertical);
	deviceContext->IASetInputLayout(NULL);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_blurShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerStateNoWrap);
	deviceContext->Draw(3, 0);

	//Vertical blur
	deviceContext->OMSetRenderTargets(1, &m_auxRTV[3], NULL);

	vertical = true;
	SetBlurShaderParam(deviceContext, m_auxSRV[2], m_viewport.Width, m_viewport.Height, vertical);
	deviceContext->IASetInputLayout(NULL);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_blurShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_samplerStateNoWrap);
	deviceContext->Draw(3, 0);

	//Final bloom image is stored in aux3
}

int PostProcessShader::GetNearestPow2(int resolution)
{
	int halfReso = resolution * 0.5f;
	//Rounded exponent
	//Adds +0.5f to round to the nearest integer
	int roundedExponent = (int)(log2(halfReso) + 0.5f);

	return pow(2, roundedExponent);
}
