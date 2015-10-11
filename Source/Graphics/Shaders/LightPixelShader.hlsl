#include "LightHelper.hlsli"

cbuffer LightBuffer
{
	DirectionalLight gDirLight;
	Material gMaterial;
	float3 gEyePosW;
	float pad;
};

cbuffer cbPerObject
{
	//Material gMaterial;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	input.normalW = normalize(input.normalW);

	float3 toEye = gEyePosW - input.positionW;
	float distToEye = length(toEye);

	toEye /= distToEye;

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]

	float4 A, D, S;
	ComputeDirectionalLight(gMaterial, gDirLight, input.normalW, toEye, A, D, S);

	ambient += A;
	diffuse += D;
	specular += S;
	
	float4 litColor = ambient + diffuse + specular;

	litColor.a = gMaterial.Diffuse.a;

	return litColor;
}