#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "../Helper/LightHelper.h"
#include "../Helper/TypeHelper.h"

class LightShader
{
public:
	LightShader();
	LightShader(const LightShader& other);
	~LightShader();

	bool Initialize(ID3D11Device* device, HWND window);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &world, 
		const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &proj, BasicLight lightData, Fog fog, DirectX::XMFLOAT3 eyePos,
		ID3D11ShaderResourceView* texture, const DirectX::XMMATRIX &textTransf,
		offsetData offset, Material material);

private:
	bool InitializeShader(ID3D11Device* device, HWND window);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &world,
		const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &proj, BasicLight lightData, Fog fog, 
		DirectX::XMFLOAT3 eyePos, ID3D11ShaderResourceView* texture, const DirectX::XMMATRIX &textTransf, Material material);
	void RenderShader(ID3D11DeviceContext* deviceContext, offsetData offset);

	struct MatrixBuffer //Used in Vertex Shader
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMFLOAT4X4 worldInvTranspose;
		DirectX::XMFLOAT4X4 texTransform;
	};

	struct LightBuffer //Used in Pixel Shader
	{
		DirectionalLight dirLight;
		PointLight pointLight;
		SpotLight spotLight;
		Material material;
		DirectX::XMFLOAT3 eyePos;
		float pad;
	};

	struct FogBuffer //Used in Pixel Shader
	{
		Fog fog;
		float pad;
	};

private:
	ID3D11SamplerState* m_samplerState;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_fogBuffer;
};