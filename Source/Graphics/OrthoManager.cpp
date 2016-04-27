#include "OrthoManager.h"
#include "../Helper/TypeHelper.h"
#include "../Helper/GeneralHelper.h"

using namespace DirectX;

OrthoManager::OrthoManager()
	:m_vertexBuffer(0),
	m_indexBuffer(0)
{
}

OrthoManager::~OrthoManager()
{
}

bool OrthoManager::Initialize(ID3D11Device * device, int width, int height)
{
	bool result;

	result = InitializeBuffers(device, width, height);
	if (!result)
	{
		return false;
	}

	return true;
}

void OrthoManager::Shutdown()
{
	ShutdownBuffers();
}

void OrthoManager::Render(ID3D11DeviceContext * deviceContext)
{
	RenderBuffers(deviceContext);
}

int OrthoManager::GetIndexCount()
{
	return m_indexCount;
}

bool OrthoManager::InitializeBuffers(ID3D11Device * device, int width, int height)
{
	HRESULT result;
	float left;
	float right;
	float top;
	float bottom;
	SpriteVertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	left = (float)((width / 2) * -1);
	right = left + (float)width;
	top = (float)(height / 2);
	bottom = top - (float)height;

	m_vertexCount = 4;
	m_indexCount = 6;
	vertices = new SpriteVertex[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//triangle 1
	/*
	vertices[0].Position = XMFLOAT3(width/2, -height/2, 0.0f);
	vertices[0].Texture = XMFLOAT2(1.0f, 1.0f);

	vertices[1].Position = XMFLOAT3(-width/2, -height/2, 0.0f);
	vertices[1].Texture = XMFLOAT2(0.0f, 1.0f);

	vertices[2].Position = XMFLOAT3(-width / 2, height / 2, 0.0f);
	vertices[2].Texture = XMFLOAT2(0.0f, 0.0f);

	//triangle 2
	vertices[3].Position = XMFLOAT3(width / 2, height / 2, 0.0f);
	vertices[3].Texture = XMFLOAT2(1.0f, 0.0f);
	*/

	vertices[0].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[0].Texture = XMFLOAT2(1.0f, 1.0f);
	vertices[1].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[1].Texture = XMFLOAT2(0.0f, 1.0f);
	vertices[2].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[2].Texture = XMFLOAT2(0.0f, 0.0f);
	vertices[3].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[3].Texture = XMFLOAT2(1.0f, 0.0f);

	/*
	vertices[4].Position = XMFLOAT3(right, top, 0.0f);
	vertices[4].Texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].Position = XMFLOAT3(right, bottom, 0.0f);
	vertices[5].Texture = XMFLOAT2(1.0f, 1.0f);
	*/

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SpriteVertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void OrthoManager::ShutdownBuffers()
{
	ReleaseCOM(m_indexBuffer);
	ReleaseCOM(m_vertexBuffer);
}

void OrthoManager::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(SpriteVertex);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
