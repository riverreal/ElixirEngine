#pragma once

#include <d3d11.h>
#include <vector>

const int G_BUFFER_COUNT = 4;

class DeferredRendering
{
public:
	DeferredRendering();
	~DeferredRendering();

	bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear);
	void Shutdown();

	void SetRenderTargets(ID3D11DeviceContext* deviceContext);
	void ClearRenderTargets(ID3D11DeviceContext* deviceContext);

	ID3D11ShaderResourceView* GetShaderResourceView(int index);
private:
	int m_textureWidth;
	int m_textureHeight;

	ID3D11Texture2D* m_gBuffer[G_BUFFER_COUNT];
	ID3D11RenderTargetView* m_gBufferRTV[G_BUFFER_COUNT];
	ID3D11ShaderResourceView* m_gBufferSRV[G_BUFFER_COUNT];
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;

	ID3D11RenderTargetView* m_albedo;
};