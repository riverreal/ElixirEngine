#pragma once

#include<d3d11.h>
#include <d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>
#include "../Helper/TypeHelper.h"
#include "CameraManager.h"
#include "../System/Object.h"

class DeferredShader
{
public:
	DeferredShader();
	~DeferredShader();

	bool Initialize(ID3D11Device* device, HWND window);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, Object* object, Camera camera);

private:
	bool InitializeShader(ID3D11Device* device, HWND window);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& texTrans, DirectX::XMMATRIX& world,
		DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection, ID3D11ShaderResourceView* albedoSRV, ID3D11ShaderResourceView* roughnessSRV, ID3D11ShaderResourceView* metallicSRV, ID3D11ShaderResourceView* normalSRV);
	void RenderShader(ID3D11DeviceContext* deviceContext, offsetData offset);

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX texTrans;
		float pad;
	};

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11Buffer* m_matrixBuffer;
};