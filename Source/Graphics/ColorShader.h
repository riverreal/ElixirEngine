#pragma once

#include "Shaders\Compiled\ColorVertexShader.h"
#include "Shaders\Compiled\ColorPixelShader.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

class ColorShader
{
public:
	ColorShader();
	ColorShader(const ColorShader& other);
	~ColorShader();

	bool Initialize(ID3D11Device* device, HWND window);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &proj);

private:
	bool InitializeShader(ID3D11Device* device, HWND window);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &proj);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};