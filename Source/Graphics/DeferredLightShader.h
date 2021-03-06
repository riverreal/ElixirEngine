#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include "../Helper/TypeHelper.h"
#include "../System/Object.h"
#include "Light.h"
#include "CameraManager.h"

using namespace std;

class DeferredLightShader
{
public:
	DeferredLightShader();
	~DeferredLightShader();

	bool Initialize(ID3D11Device* device, HWND hWnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, offsetData offset,
		ID3D11ShaderResourceView* envMap, ID3D11ShaderResourceView* irradiance, ID3D11ShaderResourceView* albedo, 
		ID3D11ShaderResourceView* normal, ID3D11ShaderResourceView* matProp, ID3D11ShaderResourceView* position, 
		ID3D11ShaderResourceView* shadowMap, Light* lighting, Camera* camera, Fog fog);

private:

	bool InitializeShader(ID3D11Device* device, HWND hWnd);
	void ShutdownShader();
	
	bool setShaderParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* albedo, 
		ID3D11ShaderResourceView* normal, ID3D11ShaderResourceView* matProp, ID3D11ShaderResourceView* position, 
		ID3D11ShaderResourceView* irradiance, ID3D11ShaderResourceView* envMap, ID3D11ShaderResourceView* shadowMap, Light* lighting, Camera* camera, Fog fog);
	void RenderShader(ID3D11DeviceContext* deviceContext, offsetData offset);

	struct ShadowMapBuffer
	{
		DirectX::XMMATRIX lightViewMatrix;
		DirectX::XMMATRIX lightProjMatrix;
		DirectX::XMMATRIX viewMatrix;
		float pad;
	};

	struct LightBufferType
	{	
		PBRDirectionalLight dirLight;

		//With padding to align 16 byte
		U32 numPointLights;
		DirectX::XMFLOAT3 pad;

		//With padding to align 16 byte
		U32 numSpotLights;
		DirectX::XMFLOAT3 pad2;

		PBRPointLight pointLight[50];
		PBRSpotLight spotLight[20];
		
	};

	struct cbPerObject
	{
		Float4Align DirectX::XMFLOAT3 eyePos;
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
	ID3D11SamplerState* m_shadowSampler;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cbPerObject;
	ID3D11Buffer* m_fogBuffer;
	ID3D11Buffer* m_shadowMapBuffer;
};