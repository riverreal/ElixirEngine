#include "LightHelper.hlsli"

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4x4 worldInvTranspose;
	float4x4 gTexTransform;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex : TEXCOORD;
};

PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.positionW = mul(input.position, worldMatrix).xyz;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.normalW = mul(input.normal, (float3x3)worldInvTranspose);

	output.tex = mul(float4(input.tex, 0.0f, 1.0f), gTexTransform).xy;

	return output;
}