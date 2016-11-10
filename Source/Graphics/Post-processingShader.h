#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "../Helper/TypeHelper.h"

class PostProcessShader
{
public:
	PostProcessShader();
	~PostProcessShader();

	bool Initialize(ID3D11Device* device, HWND hWnd, int textureWidth, int textureHeight, float screenDepth, float screenNear, D3D11_VIEWPORT defaultVP);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, offsetData offset, float dt);

	void SetPostPRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearPostPRenderTarget(ID3D11DeviceContext* deviceContext);

private:
	bool InitializeShader(ID3D11Device* device, HWND hWnd);
	//Initialize render targets for average illuminance generation
	bool InitializeAvrgIll(ID3D11Device * device, int textureWidth, int textureHeight, float screenDepth, float screenNear);
	bool InitializePostPro(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* unProcessedImage, U32 enabled);

	bool SetAdaptedShaderParam(ID3D11DeviceContext* deviceContext, float dt, U32 firstPass, ID3D11ShaderResourceView* source = 0);

	bool SetBlurShaderParam(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* imgToBlur, float textureWidth, float textureHeight, bool vertical);
	//Render to the back buffer
	void RenderShader(ID3D11DeviceContext* deviceContext, offsetData offset);

	//Render the average illuminance for eye adaptation.
	void RenderAverageIllum(ID3D11DeviceContext* deviceContext);

	void RenderAdaptedLum(ID3D11DeviceContext* deviceContext, float dt);

	void RenderBloom(ID3D11DeviceContext* deviceContext);

	//void SetAuxRenderTarget(ID3D11DeviceContext* deviceContext, int auxIndex);
	//void ClearAuxRenderTarget(ID3D11DeviceContext* deviceContext);
	//void SetViewport(int width, int height);

	//Get the nearest power of 2 when divided by 2.
	//Example:
	//Resolution = 1920
	//1920 / 2 = 960, nearlest pow2 = 1024
	int GetNearestPow2(int resolution);

private:
	struct PostProcessParam
	{
		//integer used as boolean
		U32 enabled;
		float pad[3];
	};

	struct AdaptedBuffer
	{
		//integer used as boolean
		float dt;
		U32 firstPass;
		float pad[2];
	};

	struct blurBuffer
	{
		float passNumber;
		float width;
		float height;
		float pad;
	};

	bool m_firstFrame;
	bool m_oldSRVSwap;

	//Auxiliar buffers for post processing needed for some effects like bloom
	ID3D11Texture2D* m_auxBuffers[4];
	ID3D11RenderTargetView* m_auxRTV[4];
	ID3D11ShaderResourceView* m_auxSRV[4];

	std::vector<ID3D11Texture2D*> m_avrgIllumBuffers;
	std::vector<ID3D11RenderTargetView*> m_avrgIllumRTV;
	std::vector<ID3D11ShaderResourceView*> m_avrgIllumSRVs;
	ID3D11ShaderResourceView* m_avrgIllumSRV;

	
	ID3D11Texture2D* m_oldAdaptedIllumBuffer[2];
	ID3D11RenderTargetView* m_oldAdaptedIllumRTV[2];
	//old adapted illum
	//0 slot is the one that feeds (SetShaderResources)
	//1 slot is the one that gets the last illum (SV_Target1)
	//After render these swap. Resulting in [0] being the output.
	ID3D11ShaderResourceView* m_oldAdaptedIllumSRV[2];

	D3D11_VIEWPORT m_viewport;
	D3D11_VIEWPORT m_defaultVP;
	std::vector<U32> m_viewportX;
	std::vector<U32> m_viewportY;
	U32 m_originalX;
	U32 m_originalY;

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	//pixel shader that renders adapted luminance
	ID3D11PixelShader* m_adaptLumShader;
	ID3D11PixelShader* m_brightPassShader;
	ID3D11PixelShader* m_blurShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_samplerState;
	ID3D11SamplerState* m_samplerStateNoWrap;
	ID3D11Buffer* m_postPParamBufer;
	ID3D11Buffer* m_adaptedLumBuffer;
	ID3D11Buffer* m_blurBuffer;
};