#pragma once

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

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

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }
	
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Reflect;
};

struct Fog
{
	Fog() { ZeroMemory(this, sizeof(this)); }

	bool Enabled;
	float FogStart;
	float FogRange;
	XMFLOAT4 FogColor;

};