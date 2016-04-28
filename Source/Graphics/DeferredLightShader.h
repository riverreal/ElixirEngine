#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include "../Helper/TypeHelper.h"
#include "../Helper/LightHelper.h"
#include "../System/Object.h"

using namespace std;

class DeferredLightShader
{
public:
	DeferredLightShader();
	~DeferredLightShader();

	bool Initialize(ID3D11Device* device, HWND hWnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, offsetData offset,
		Object* object, ID3D11ShaderResourceView* albedo, ID3D11ShaderResourceView* normal, ID3D11ShaderResourceView* matProp, ID3D11ShaderResourceView* position, BasicLight lighting, DirectX::XMFLOAT3 eyePos, Fog fog);

private:

	bool InitializeShader(ID3D11Device* device, HWND hWnd);
	void ShutdownShader();
	
	bool setShaderParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* albedo, ID3D11ShaderResourceView* normal, ID3D11ShaderResourceView* matProp, ID3D11ShaderResourceView* position, ID3D11ShaderResourceView* irradiance, ID3D11ShaderResourceView* envMap, BasicLight lighting, DirectX::XMFLOAT3 eyePos, Fog fog);
	void RenderShader(ID3D11DeviceContext* deviceContext, offsetData offset);

	struct LightBufferType
	{
		DirectionalLight dirLight;
		PointLight pointLight;
		SpotLight spotLight;
		DirectX::XMFLOAT3 eyePos;
		float pad;
	};

	struct FogBuffer
	{
		Fog fog;
		float pad;
	};

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_samplerState;	
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_fogBuffer;
};