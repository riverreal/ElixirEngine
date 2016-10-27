#pragma once

#include <d3d11.h>
#include <vector>

const int G_BUFFER_COUNT = 4;

class DeferredRendering
{
public:
	DeferredRendering();
	~DeferredRendering();

	///G-Buffer Initialize:
	///Needs textureWidth, textureHeight to be evaluaded with Spec G-Buffer dimensions.
	///Only supports 16:9 aspect ratio at the moment. Other aspect ratio might experiment issues.
	///Use to evaluate dimension BaseApp::GetSpecResolution.
	bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear, int spec);
	void Shutdown();

	void SpecDefiner(int screenWidth, int screenHeight, int spec);

	void SetRenderTargets(ID3D11DeviceContext* deviceContext);
	void ClearRenderTargets(ID3D11DeviceContext* deviceContext);

	void SetPostpRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearPostpRenderTarget(ID3D11DeviceContext* deviceContext);

	ID3D11ShaderResourceView* GetShaderResourceView(int index);

	ID3D11ShaderResourceView* GetPostpSRV();

private:
	//Stores the spec version of the screen width
	int m_textureWidth;
	//Stores the spec version of the screen height;
	int m_textureHeight;

	//Stores the original screen width
	int m_realWidth;
	//Stores the original screen height
	int m_realHeight;
	

	ID3D11Texture2D* m_gBuffer[G_BUFFER_COUNT];
	ID3D11RenderTargetView* m_gBufferRTV[G_BUFFER_COUNT];
	ID3D11ShaderResourceView* m_gBufferSRV[G_BUFFER_COUNT];
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;

	//post processing buffer
	ID3D11Texture2D* m_postpBuffer;
	//post processing rtv
	ID3D11RenderTargetView* m_postpRTV;
	//post processing srv
	ID3D11ShaderResourceView* m_postpSRV;
	//post processing depth stencil buffer
	ID3D11Texture2D* m_ppdepthStencilBuffer;
	//post processing depth stencil view
	ID3D11DepthStencilView* m_ppdepthStencilView;

	ID3D11RenderTargetView* m_albedo;
};