#include "ShadowMap.h"
#include "../Helper/GeneralHelper.h"

ShadowMap::ShadowMap()
	:m_width(0),
	m_height(0),
	m_depthMapDSV(0),
	m_depthMapSRV(0)
{
}

ShadowMap::~ShadowMap()
{
}

bool ShadowMap::Initialize(ID3D11Device * device, UINT width, UINT height)
{
	//Assing values
	m_width = width;
	m_height = height;

	if(!InitializeSM(device))
	{
		RadixLog("Failed to initialize Shadow Map");
		return false;
	}

	return true;
}

void ShadowMap::Render(ID3D11DeviceContext * deviceContext)
{
	RenderSM(deviceContext);
}

void ShadowMap::Shutdown()
{
	ReleaseCOM(m_depthMapSRV);
	ReleaseCOM(m_depthMapDSV);
}

ID3D11ShaderResourceView * ShadowMap::GetShadowMap()
{
	return m_depthMapSRV;
}

bool ShadowMap::InitializeSM(ID3D11Device* device)
{
	HRESULT result;


	//Create depth map texture--------------------------------------
	D3D11_TEXTURE2D_DESC textDesc;

	textDesc.Width = m_width;
	textDesc.Height = m_height;
	textDesc.MipLevels = 1;
	textDesc.ArraySize = 1;
	textDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	textDesc.SampleDesc.Count = 1;
	textDesc.SampleDesc.Quality = 0;
	textDesc.Usage = D3D11_USAGE_DEFAULT;
	textDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textDesc.CPUAccessFlags = 0;
	textDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = 0;

	result = device->CreateTexture2D(&textDesc, 0, &depthMap);
	if (FAILED(result))
	{
		RadixLog("Failed creating depth map texture");
		return false;
	}

	//Create depth stencil view--------------------------------------
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depthMap, &dsvDesc, &m_depthMapDSV);
	if (FAILED(result))
	{
		RadixLog("Failed to create depth stencil view in ShadowMap");
		return false;
	}

	//Create depth map shader resource view--------------------------------------
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	result = device->CreateShaderResourceView(depthMap, &srvDesc, &m_depthMapSRV);
	if (FAILED(result))
	{
		RadixLog("Failed to create shader resource view in ShadowMap");
		return false;
	}

	//D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;

	ReleaseCOM(depthMap);

	//Initialize viewport--------------------------------------
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = m_width;
	m_viewport.Height = m_height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	return true;
}

void ShadowMap::RenderSM(ID3D11DeviceContext* deviceContext)
{
	deviceContext->RSSetViewports(1, &m_viewport);

	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	deviceContext->OMSetRenderTargets(1, renderTargets, m_depthMapDSV);

	deviceContext->ClearDepthStencilView(m_depthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
