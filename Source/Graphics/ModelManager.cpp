#include "ModelManager.h"
#include "../Helper/GeneralHelper.h"
#include <fstream>
#include <string>
#include <iostream>

using namespace DirectX;

Model::Model()
	:m_vertexBuffer(0),
	m_indexBuffer(0),
	m_totalVertexCount(0),
	m_totalIndexCount(0),
	m_lastVertexOffset(0),
	m_lastVertexSize(0),
	m_lastIndexOffset(0),
	m_lastIndexSize(0)
{
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

offsetData Model::AddGeometry(int modelType)
{
	offsetData offset;
	Terrain plainTerrain;
	BasicShapes shapes;
	MeshData meshData;
	UINT currentVertexCount;
	UINT currentIndexCount;

	switch (modelType)
	{
	case MODEL_TYPE_CUBE:
		shapes.CreateCube(1.0f, 1.0f, 1.0f, meshData);
		break;
	case MODEL_TYPE_CYLINDER:
		shapes.CreateCylinder(0.15f, 0.15f, 3.0f, 10, 10, meshData);
		break;
	case MODEL_TYPE_GEOSPHERE:
		shapes.CreateGeosphere(0.5f, 2, meshData);
		break;
	case MODEL_TYPE_PLAIN:
		plainTerrain.CreatePlane(20.0f, 30.0f, 60, 40, meshData);
		break;
	case MODEL_TYPE_SCREEN_LAYER:
		break;
	case MODEL_TYPE_SPHERE:
		shapes.CreateSphere(0.5f, 20, 20, meshData);
		break;
	default:
		shapes.CreateCube(1.0f, 1.0f, 1.0f, meshData);
		break;
	}

	currentVertexCount = meshData.Vertices.size();
	currentIndexCount = meshData.Indices.size();
	m_totalVertexCount += meshData.Vertices.size();
	m_totalIndexCount += meshData.Indices.size();

	std::vector<Vertex> vertices(currentVertexCount);
	for (size_t i = 0; i < currentVertexCount; ++i)
	{
		XMFLOAT3 position = meshData.Vertices[i].Position;
		vertices[i].Position = position;
		vertices[i].Normal = meshData.Vertices[i].Normal;
		m_totalVertex.push_back(vertices[i]);
	}

	std::vector<UINT> indices(currentIndexCount);
	for (size_t i = 0; i < currentIndexCount; ++i)
	{
		indices[i] = meshData.Indices[i];
		m_totalIndex.push_back(indices[i]);
	}

	//get the index offset
	m_lastIndexOffset += m_lastIndexSize;
	m_lastIndexSize = indices.size();

	//get the vertex offset
	m_lastVertexOffset += m_lastVertexSize;
	m_lastVertexSize = vertices.size();

	offset.indexCount = currentIndexCount;
	offset.indexOffset = m_lastIndexOffset;
	offset.vertexOffset = m_lastVertexOffset;

	return offset;
}

offsetData Model::AddCustomGeometry(std::wstring fileName)
{
	offsetData offset;
	UINT vertexCount = 0;
	UINT trianglesCount = 0;
	std::string ignore;
	std::ifstream fin;
	UINT indexCount;

	std::string str_fileName(fileName.begin(), fileName.end());
	//Get Data from File
	fin.open(str_fileName);
	LPCWSTR errorMsg = L"Not Found";

	if (!fin)
	{
		MessageBox(0, fileName.c_str(), errorMsg, 0);
		return offset;
	}

	fin >> ignore >> vertexCount;
	fin >> ignore >> trianglesCount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::cout << vertexCount;

	float nx, ny, nz;
	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	std::vector<Vertex> vertices(vertexCount);
	for (UINT i = 0; i < vertexCount; ++i)
	{
		fin >> vertices[i].Position.x >> vertices[i].Position.y >> vertices[i].Position.z;

		fin >> nx >> ny >> nz; //normals

		vertices[i].Normal = XMFLOAT3(nx,ny,nz);

		m_totalVertex.push_back(vertices[i]);
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	indexCount = 3 * trianglesCount;
	std::vector<UINT> indices(indexCount);
	for (UINT i = 0; i < trianglesCount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	for (UINT i = 0; i < indexCount; ++i)
	{
		m_totalIndex.push_back(indices[i]);
	}

	fin.close();

	//get the vertex and index count
	m_totalVertexCount += vertexCount;
	m_totalIndexCount += indexCount;

	//get the index offset
	m_lastIndexOffset += m_lastIndexSize;
	m_lastIndexSize = indices.size();

	//get the vertex offset
	m_lastVertexOffset += m_lastVertexSize;
	m_lastVertexSize = vertices.size();

	//Calculate Offset
	offset.indexCount = indexCount;
	offset.indexOffset = m_lastIndexOffset;
	offset.vertexOffset = m_lastVertexOffset;

	return offset;
}

bool Model::Initialize(ID3D11Device* device)
{
	if (!InitializeBuffers(device))
	{
		return false;
	}

	return true;
}

void Model::Shutdown()
{
	ShutdownBuffers();
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int Model::GetIndexCount()
{
	return m_totalIndexCount;
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	//-----------------------------------------------------
	//Needed Variables
	//-----------------------------------------------------
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	HRESULT result;

	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Vertex)* m_totalVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = &m_totalVertex[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_totalIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = &m_totalIndex[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	
	return true;
}

void Model::ShutdownBuffers()
{
	ReleaseCOM(m_vertexBuffer);
	ReleaseCOM(m_indexBuffer);
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride;
	UINT offset;

	stride = sizeof(Vertex);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}