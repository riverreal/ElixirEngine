#include "LightHelper.hlsli"

cbuffer LightBuffer
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	Material gMaterial;
	float3 gEyePosW;
	float pad;
};

cbuffer FogBuffer
{
	Fog gFog;
	//float pad;
};

Texture2D gSpriteArray;
SamplerState samAnisotropic;

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 posW : POSITION;
	float3 NormalW: NORMAL;
	float2 Tex : TEXCOORD;
	uint PrimID : SV_PRIMITIVEID;
};

float4 PS(GeoOut input) : SV_TARGET
{
	input.normalW = normalize(input.normalW);

	float3 toEye = gEyePosW - input.positionW;

	float distToEye = length(toEye);
	toEye /= distToEye;

	float4 texColor = float4(1, 1, 1, 1);
	float3 uvw = float3(input.Tex, input.PrimID%4);
	texColor = gSpriteArray.Sample(samAnisotropic, input.tex);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]

	float4 A, D, S;

	ComputeDirectionalLight(gMaterial, gDirLight, input.normalW, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	ComputePointLight(gMaterial, gPointLight, input.positionW, input.normalW, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	ComputeSpotLight(gMaterial, gSpotLight, input.positionW, input.normalW, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	//Modelulate with late add
	float4 litColor = texColor * (ambient + diffuse) + specular;

	//fog
	if (gFog.Enabled == true)
	{
		float fogLerp = saturate((distToEye - gFog.FogStart) / gFog.FogRange);
		litColor = lerp(litColor, gFog.FogColor, fogLerp);
	}

	litColor.a = gMaterial.Diffuse.a * texColor.a;

	return litColor;
}