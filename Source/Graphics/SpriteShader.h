#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "../Helper/LightHelper.h"
#include "../Helper/TypeHelper.h"

class SpriteShader
{
public:
	SpriteShader();
	~SpriteShader();

	bool Initialize(ID3D11Device* device, HWND window, Material material);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &proj, BasicLight lightData, Fog fog, DirectX::XMFLOAT3 eyePos,
		ID3D11ShaderResourceView* texture, UINT vertexCountOffset);

private:
	bool InitializeShader(ID3D11Device* device, HWND window);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &proj, BasicLight lightData, Fog fog, DirectX::XMFLOAT3 eyePos, ID3D11ShaderResourceView* texture);
	void RenderShader(ID3D11DeviceContext* deviceContext, UINT vertexCountOffset);

	struct MatrixBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
		DirectX::XMFLOAT3 eyePos;
		float pad;
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
	ID3D11GeometryShader* m_geometryShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_fogBuffer;

	Material m_material;
};