#include "DeferredRendering.h"
#include "../Helper/GeneralHelper.h"

DeferredRendering::DeferredRendering()
{
	for (int i = 0; i < G_BUFFER_COUNT; ++i)
	{
		m_gBuffer[i] = 0;
		m_gBufferRTV[i] = 0;
		m_gBufferSRV[i] = 0;
	}

	m_depthStencilBuffer = 0;
	m_depthStencilView = 0;
}

DeferredRendering::~DeferredRendering()
{
}

bool DeferredRendering::Initialize(ID3D11Device * device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	D3D11_TEXTURE2D_DESC texDesc;
	HRESULT result;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	int i;

	m_textureWidth = textureWidth;
	m_textureHeight = textureHeight;

	ZeroMemory(&texDesc, sizeof(texDesc));
	
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

	for (int i = 0; i < G_BUFFER_COUNT; ++i)
	{
		result = device->CreateTexture2D(&texDesc, NULL, &m_gBuffer[i]);
		if (FAILED(result))
		{
			return false;
		}
	}

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < G_BUFFER_COUNT; ++i)
	{
		result = device->CreateRenderTargetView(m_gBuffer[i], &rtvDesc, &m_gBufferRTV[i]);
		if (FAILED(result))
		{
			return false;
		}
	}

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < G_BUFFER_COUNT; ++i)
	{
		result = device->CreateShaderResourceView(m_gBuffer[i], &srvDesc, &m_gBufferSRV[i]);
		if (FAILED(result))
		{
			return false;
		}
	}

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(m_depthStencilBuffer, &dsvDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void DeferredRendering::Shutdown()
{
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilBuffer);

	for (int i = 0; i < G_BUFFER_COUNT; ++i)
	{
		ReleaseCOM(m_gBufferSRV[i]);
		ReleaseCOM(m_gBufferRTV[i]);
		ReleaseCOM(m_gBuffer[i]);
	}
}

void DeferredRendering::SetRenderTargets(ID3D11DeviceContext * deviceContext)
{
	deviceContext->OMSetRenderTargets(G_BUFFER_COUNT, m_gBufferRTV, m_depthStencilView);
}

void DeferredRendering::ClearRenderTargets(ID3D11DeviceContext * deviceContext)
{
	float color[4] = {0.0f, 0.0f, 1.0f, 1.0f};

	for (int i = 0; i < G_BUFFER_COUNT; ++i)
	{
		deviceContext->ClearRenderTargetView(m_gBufferRTV[i], color);
	}
	
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView * DeferredRendering::GetShaderResourceView(int index)
{
	return m_gBufferSRV[index];
}