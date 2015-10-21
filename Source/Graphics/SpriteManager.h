#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "../Helper/TypeHelper.h"
#include <vector>

class Sprite
{
public:
	Sprite();
	~Sprite();

	///<summary>
	///
	///Adds as many sprites as spriteCount to the vertex buffer
	///</summary>
	UINT AddSprite(UINT spriteCount);
	UINT GetTotalVertexCount();

	bool Initialize(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ID3D11Buffer* m_vertexBuffer;
	UINT m_totalVertexCount;
	std::vector<SpriteVertex> m_totalVertex;

	UINT m_lastVertexOffset;
	UINT m_lastVertexSize;
};