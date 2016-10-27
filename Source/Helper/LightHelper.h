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
	PBRSpotLight(float posX, float posY, float posZ, float colorR, float colorG, float colorB, float range, float spot, float constAtt, float linearAtt, float quadAtt, float dirX, float dirY, float dirZ)
		:Position(XMFLOAT3(posX, posY, posZ)),
		LightColor(XMFLOAT4(colorR, colorG, colorB, 1.0f)),
		Range(range),
		Spot(spot),
		Att(XMFLOAT3(constAtt, linearAtt, quadAtt)),
		Direction(XMFLOAT3(dirX, dirY, dirZ))
	{
	}

	XMFLOAT4 LightColor;

	XMFLOAT3 Position;
	//Range of the light
	float Range;

	//Direction of the spot light
	XMFLOAT3 Direction;
	//Exponent that controls the spotlight cone (angle)
	float Spot;

	//Three attenuation constants that controls the fall off with distance
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


