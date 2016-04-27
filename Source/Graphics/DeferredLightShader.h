#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include "../Helper/TypeHelper.h"

using namespace std;

class DeferredLightShader
{
public:
	DeferredLightShader();
	~DeferredLightShader();

	bool Initialize(ID3D11Device* device, HWND hWnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, offsetData offset, DirectX::XMMATRIX& world, DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection,
		ID3D11ShaderResourceView* albedo, ID3D11ShaderResourceView* normal, DirectX::XMFLOAT3 lightDirection);

private:

	bool InitializeShader(ID3D11Device* device, HWND hWnd);
	void ShutdownShader();
	
	bool setShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& world, DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection,
		ID3D11ShaderResourceView* albedo, ID3D11ShaderResourceView* normal, DirectX::XMFLOAT3 lightDirection);
	void RenderShader(ID3D11DeviceContext* deviceContext, offsetData offset);

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct LightBufferType
	{
		DirectX::XMFLOAT3 lightDirection;
		float padding;
	};

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_samplerState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
};