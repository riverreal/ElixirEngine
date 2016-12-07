#pragma once

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

class MathHelper
{
public:
	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	static float AngleFromXY(float x, float y);

	static XMMATRIX WorldToBillboard(const XMMATRIX &source, XMFLOAT3 eyePos);

	static const float Pi;

	static float lerp(float a, float b, float x);
	static XMFLOAT3 lerp(XMFLOAT3 a, XMFLOAT3 b, float x);

	static XMFLOAT3 F3DotScalar(XMFLOAT3 x, float s);
	static XMFLOAT3 F3MinusF3(XMFLOAT3 a, XMFLOAT3 b);

};