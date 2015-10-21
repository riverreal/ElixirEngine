#include "SpriteManager.h"
#include "../Helper/GeneralHelper.h"

Sprite::Sprite()
	:m_vertexBuffer(0),
	m_totalVertexCount(0),
	m_lastVertexOffset(0),
	m_lastVertexSize(0)
{
}

Sprite::~Sprite()
{
}

UINT Sprite::GetTotalVertexCount()
{
	return m_totalVertexCount;
}

UINT Sprite::AddSprite(UINT spriteCount)
{
	std::vector<SpriteVertex> vertices(spriteCount);

	for (UINT i = 0; i < spriteCount; ++i)
	{
		vertices[i].Pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertices[i].Size = DirectX::XMFLOAT2(1.0f, 1.0f);

		m_totalVertex.push_back(vertices[i]);
	}

	m_lastVertexOffset += m_lastVertexSize;
	m_lastVertexSize = vertices.size();
	m_totalVertexCount += spriteCount;

	return m_lastVertexOffset;
}

bool Sprite::Initialize(ID3D11Device* device)
{
	if (!InitializeBuffers(device))
	{
		return false;
	}

	return true;
}

void Sprite::Shutdown()
{
	ShutdownBuffers();
}

void Sprite::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

bool Sprite::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	HRESULT result;

	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(SpriteVertex)* m_totalVertexCount;
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
}

void Sprite::ShutdownBuffers()
{
	ReleaseCOM(m_vertexBuffer);
}

void Sprite::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}