#pragma once

#include "../Helper/GeneralHelper.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Terrain.h"

enum BasicModel
{
	MODEL_TYPE_CUBE,
	MODEL_TYPE_SPHERE,
	MODEL_TYPE_GEOSPHERE,
	MODEL_TYPE_CYLINDER,
	MODEL_TYPE_PLAIN
};

class Model
{
public:
	Model(int modelType);
	Model(const Model& other);
	~Model();

	bool Initialize(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

private:
	int m_modelType;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
};