#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Terrain.h"
#include "BasicShapes.h"

enum BasicModel
{
	MODEL_TYPE_CUBE,
	MODEL_TYPE_SPHERE,
	MODEL_TYPE_GEOSPHERE,
	MODEL_TYPE_CYLINDER,
	MODEL_TYPE_PLAIN,
	MODEL_TYPE_SCREEN_LAYER
};

struct offsetData
{
	UINT indexCount;
	UINT indexOffset;
	UINT vertexOffset;
};

class Model
{
public:
	Model();
	Model(const Model& other);
	~Model();

	offsetData AddGeometry(int modelType);
	offsetData AddCustomGeometry(std::wstring fileName);
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
	int m_totalVertexCount;
	int m_totalIndexCount;
	std::vector<VertexType> m_totalVertex;
	std::vector<UINT> m_totalIndex;

	UINT m_lastVertexOffset;
	UINT m_lastVertexSize;
	UINT m_lastIndexOffset;
	UINT m_lastIndexSize;
};