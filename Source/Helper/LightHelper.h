#pragma once

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

#define Float4Align __declspec(align(16))

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float pad;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Att;
	float pad;
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Direction;
	float Spot;

	XMFLOAT3 Att;
	float pad;
};

struct BasicLight
{
	BasicLight() { ZeroMemory(this, sizeof(this)); }
	DirectionalLight Directional;
	PointLight Point;
	SpotLight Spot;
};

struct PBRDirectionalLight
{
	PBRDirectionalLight() { ZeroMemory(this, sizeof(this)); }
	float LightColor[4];
	float LightIntensity[4]; //Light Intensity, ambient intensity

	float Direction[3];
	float pad;
};

struct PBRPointLight
{
	PBRPointLight() { ZeroMemory(this, sizeof(this)); }
	PBRPointLight(float posX, float posY, float posZ, float colorR, float colorG, float colorB, float range)
		:Position(XMFLOAT3(posX, posY, posZ)),
		LightColor(XMFLOAT4(colorR, colorG, colorB, 1.0f)),
		Range(range)
	{}
	XMFLOAT4 LightColor;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Att;
	float pad;
};

struct PBRSpotLight
{
	PBRSpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 LightColor;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Direction;
	float Spot;

	XMFLOAT3 Att;
	float pad;
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }
	
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Properties; //X = Reflectiveness, Y = Metallic, Z = Roughness, W = unused
};

struct Fog
{
	Fog() { ZeroMemory(this, sizeof(this)); }

	bool Enabled;
	float FogStart;
	float FogRange;
	XMFLOAT4 FogColor;

};


