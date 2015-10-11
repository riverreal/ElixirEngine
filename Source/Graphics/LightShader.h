#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "../Helper/LightHelper.h"

class LightShader
{
public:
	LightShader();
	LightShader(const LightShader& other);
	~LightShader();

	bool Initialize(ID3D11Device* device, HWND window, Material material);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &world, 
		const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &proj, DirectionalLight dirLight, DirectX::XMFLOAT3 eyePos,
		UINT indexCount, UINT indexOffset = 0, UINT vertexOffset = 0);

private:
	bool InitializeShader(ID3D11Device* device, HWND window);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &world,
		const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &proj, DirectionalLight dirLight, DirectX::XMFLOAT3 eyePos);
	void RenderShader(ID3D11DeviceContext* deviceContext, UINT indexCount, UINT indexOffset, UINT vertexOffset);

	struct MatrixBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMFLOAT4X4 worldInvTranspose;
	};

	struct LightBuffer
	{
		DirectionalLight dirLight;
		Material material;
		DirectX::XMFLOAT3 eyePos;
		float pad;
	};

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	Material m_material;
};