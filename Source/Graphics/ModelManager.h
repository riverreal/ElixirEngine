#pragma once

#pragma comment(lib, "assimp.lib")

#include <d3d11.h>
#include <DirectXMath.h>
#include "Terrain.h"
#include "BasicShapes.h"
#include "../../External Soruce/Assimp/include/assimp/Importer.hpp"
#include "../../External Soruce/Assimp/include/assimp/scene.h"
#include "../../External Soruce/Assimp/include/assimp/postprocess.h"

class Model
{
public:
	Model();
	Model(const Model& other);
	~Model();

	offsetData AddGeometry(int modelType);
	offsetData AddCustomGeometry(std::wstring fileName);
	offsetData AddModelFromFile(std::string fileName);
	bool Initialize(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	int m_modelType;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	UINT m_totalVertexCount;
	UINT m_totalIndexCount;
	std::vector<Vertex> m_totalVertex;
	std::vector<UINT> m_totalIndex;

	UINT m_lastVertexOffset;
	UINT m_lastVertexSize;
	UINT m_lastIndexOffset;
	UINT m_lastIndexSize;
};