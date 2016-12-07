#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "../System/Object.h"
#include "CameraManager.h"
#include "../Helper/LightHelper.h"
#include "../Helper/TypeHelper.h"
#include "Light.h"

class ShadowMapShader
{
public:
	ShadowMapShader();
	~ShadowMapShader();

	bool Initialize(ID3D11Device* device);
	void Render(ID3D11DeviceContext* deviceContext, Object* object, Light* light);
	void Shutdown();

private:
	bool InitializeShader(ID3D11Device* device);
	void RenderShader(ID3D11DeviceContext* deviceContext, Object* object, Light* light);

	struct DepthMatrixBuffer
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
	ID3D11Buffer* m_depthMatrixBuffer;
};