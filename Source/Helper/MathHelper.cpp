#include "MathHelper.h"
#include <cmath>

const float MathHelper::Pi = 3.1415926535f;


float MathHelper::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	if (x >= 0.0f)
	{
		theta = atanf(y / x);

		if (theta < 0.0f)
		{
			theta += 2.0f*Pi;
		}
	}
	else
	{
		theta = atanf(y / x) + Pi;
	}

	return theta;
}

XMMATRIX MathHelper::WorldToBillboard(const XMMATRIX &source, XMFLOAT3 eyePos)
{
	XMMATRIX billboardMatrix;
	float angle;
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, source.r[3]);

	angle = atan2(pos.x - eyePos.x, pos.z - eyePos.z);
	billboardMatrix = source;
	billboardMatrix = XMMatrixMultiply(billboardMatrix, XMMatrixRotationY(XMConvertToRadians( angle)));
	return billboardMatrix;
}

float MathHelper::lerp(float a, float b, float x)
{
	return a + x * (b - a);
}

XMFLOAT3 MathHelper::lerp(XMFLOAT3 a, XMFLOAT3 b, float x)
{
	return XMFLOAT3(lerp(a.x, b.x, x), lerp(a.y, b.y, x), lerp(a.z, b.z, x));
}

XMFLOAT3 MathHelper::F3DotScalar(XMFLOAT3 x, float s)
{
	return XMFLOAT3(x.x*s, x.y*s, x.z*s);
}

XMFLOAT3 MathHelper::F3MinusF3(XMFLOAT3 a, XMFLOAT3 b)
{
	return XMFLOAT3(a.x-b.x, a.y-b.y, a.z-b.z);
}

