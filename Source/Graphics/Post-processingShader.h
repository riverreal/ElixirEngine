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

	bool Initialize(ID3D11Device* device, HWND hWnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, offsetData offset, ID3D11ShaderResourceView* unProcessedImage);

private:
	bool InitializeShader(ID3D11Device* device, HWND hWnd);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* unProcessedImage);
	void RenderShader(ID3D11DeviceContext* deviceContext, offsetData offset);

	

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_samplerState;
};