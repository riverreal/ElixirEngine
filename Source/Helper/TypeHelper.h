#pragma once

#include <DirectXMath.h>
#include <vector>

typedef unsigned int UINT;
typedef unsigned long DWORD;

struct Vertex2
{
	Vertex2() {}
	Vertex2(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT3& t, const DirectX::XMFLOAT2& uv)
		:Position(p), Normal(n), TangentU(t), TexC(uv) {}
	Vertex2(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		:Position(px, py, pz), Normal(nx, ny, nz),
		TangentU(tx, ty, tz), TexC(u, v) {}

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
	DirectX::XMFLOAT3 TangentU;
	
};

struct Vertex
{
	Vertex() {}
	Vertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT3& t, const DirectX::XMFLOAT2& uv)
		:Position(p), Normal(n), TangentU(t), Tex(uv) {}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		:Position(px, py, pz), Normal(nx, ny, nz), Tex(u, v) {}

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 Tex;
	DirectX::XMFLOAT3 TangentU;

};

struct SpriteVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 Texture;
};

struct MeshData
{
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
};

struct offsetData
{
	offsetData() {}
	offsetData(UINT iCount, UINT iOffset, UINT vOffset)
		:indexCount(iCount), indexOffset(iOffset), vertexOffset(vOffset) {}
	UINT indexCount;
	UINT indexOffset;
	UINT vertexOffset;
};