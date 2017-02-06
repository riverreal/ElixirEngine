#pragma once

#include <DirectXMath.h>
#include <vector>

typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef unsigned int U32;
typedef int I32;
//32-bit float
typedef float F32;

enum BasicModel
{
	MODEL_TYPE_CUBE,
	MODEL_TYPE_SPHERE,
	MODEL_TYPE_GEOSPHERE,
	MODEL_TYPE_CYLINDER,
	MODEL_TYPE_PLAIN,
	MODEL_TYPE_SCREEN_LAYER
};

namespace Elixir
{
	//A Vector storing 3 floats
	struct Vec3f
	{
		Vec3f()
			:x(0.0f), y(0.0f), z(0.0f)
		{};

		Vec3f(F32 x, F32 y, F32 z)
			:x(x), y(y), z(z)
		{};

		Vec3f(F32 xyz)
			:x(xyz), y(xyz), z(xyz)
		{};

		Vec3f(DirectX::XMFLOAT3 xmf3)
			:x(xmf3.x), y(xmf3.y), z(xmf3.z)
		{};

		F32 x;
		F32 y;
		F32 z;
	};

	//A Vector storing 4 floats
	struct Vec4f
	{
		Vec4f()
			:x(0.0f), y(0.0f), z(0.0f), w(0.0f)
		{};

		Vec4f(F32 x, F32 y, F32 z, F32 w)
			:x(x), y(y), z(z), w(w)
		{};

		Vec4f(F32 xyzw)
			:x(xyzw), y(xyzw), z(xyzw), w(xyzw)
		{};

		F32 x;
		F32 y;
		F32 z;
		F32 w;
	};

	//An element with a string and an ID
	struct StringID
	{
		StringID() {};

		StringID(std::string n, U32 i)
			:
			name(n), ID(i)
		{}

		std::string name;
		U32 ID;
	};

	struct TransformData
	{
		TransformData()
			:exists(false)
		{};

		bool exists;

		bool dynamic;
		Vec3f pos;
		Vec3f rot;
		Vec3f sca;
		Vec3f tpos;
		Vec3f trot;
		Vec3f tsca;
	};

	struct RendererData
	{
		RendererData()
			:exists(false)
		{};

		bool exists;

		bool enabled;
		bool backface;
		bool castShadow;

		//IDs
		U32 alb;
		U32 norm;
		U32 rou;
		U32 met;

		std::string modelPath;
		bool modelTypePrimitive;
		U32 primitiveType;
	};

	struct ObjectID
	{
		U32 ID;
		TransformData transform;
		RendererData renderer;
	};

	struct ProjectData
	{
		std::vector<StringID> TextureData;
		std::vector<StringID> MaterialData;
		std::vector<StringID> SceneData;
	};

	struct SceneData
	{
		std::string name;
		std::vector<ObjectID> GameObjects;
	};

	enum OBJECT_PRESET
	{
		//Empty object. With no components added.
		OBJECT_EMPTY,
		//Object with Transform component added.
		OBJECT_TRANSFORM,
		//Object with Transform/Renderer3D components added.
		OBJECT_RENDER
	};
	

	struct Material
	{
		Material()
			:albedo(1), normal(2), roughness(3), metallic(4), name("NewMaterial")
		{};

		//Set material
		Material(U32 alb, U32 norm, U32 rough, U32 metal)
			:albedo(alb), normal(norm), roughness(rough), metallic(metal), name("NewMaterial")
		{};

		//Copy material
		Material(const Material &cpy)
			:albedo(cpy.albedo), normal(cpy.normal), roughness(cpy.roughness), metallic(cpy.metallic), name(cpy.name)
		{};

		// 0 means unset
		U32 albedo;
		U32 normal;
		U32 roughness;
		U32 metallic;

		U32 materialID;

		std::string name;
	};
}

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
		:Position(px, py, pz), Normal(nx, ny, nz),
		TangentU(tx, ty, tz), Tex(u, v) {}

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
	offsetData() 
		:vertexOffset(0), indexCount(0), indexOffset(0)
	{}
	offsetData(UINT iCount, UINT iOffset, UINT vOffset)
		:indexCount(iCount), indexOffset(iOffset), vertexOffset(vOffset) {}
	UINT indexCount;
	UINT indexOffset;
	UINT vertexOffset;
};

struct BoundingSphere
{
	BoundingSphere()
		:Center(0.0f, 0.0f, 0.0f),
		Radius(0.0f)
	{}

	BoundingSphere(float x, float y, float z, float r)
		:Center(x, y, z),
		Radius(r)
	{}

	DirectX::XMFLOAT3 Center;
	float Radius;
};

enum RENDER_MODE
{
	DEFERRED_RENDERING,
	FOREWARD_RENDERING
};

enum TEXTURE_TYPE
{
	ALBEDO,
	IRRADIANCE,
	ENV_MAP,
	ROUGHNESS,
	METALLIC,
	NORMAL
};