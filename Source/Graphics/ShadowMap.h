#pragma once

#include <d3d11.h>

class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	bool Initialize(ID3D11Device* device, UINT width, UINT height);
	void Render(ID3D11DeviceContext* deviceContext);
	void Shutdown();

	//Returns the depth map generated from the scene.
	ID3D11ShaderResourceView* GetShadowMap();

private:
	bool InitializeSM(ID3D11Device* device);
	//Sets render target to null since depth is the only thing needed.
	void RenderSM(ID3D11DeviceContext* deviceContext);

private:
	UINT m_width;
	UINT m_height;

	ID3D11ShaderResourceView* m_depthMapSRV;
	ID3D11DepthStencilView* m_depthMapDSV;
	//ID3D11RenderTargetView* m_depthMapRTV;

	D3D11_VIEWPORT m_viewport;
};