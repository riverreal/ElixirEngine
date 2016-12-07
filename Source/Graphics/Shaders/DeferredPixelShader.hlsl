#include "GeneralHelper.hlsli"

Texture2D gAlbedo : register(t0);
Texture2D gRoughness : register(t1);
Texture2D gMetallic : register(t2);
Texture2D gNormalMap : register(t3);

SamplerState samAnisotropic : register(s0);

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex : TEXCOORD0;
	float3 tangentW : TANGENT;
};

struct PixelOutputType
{
	float4 albedo : SV_Target0;
	float4 normal : SV_Target1;
	float4 materialProp : SV_Target2;
	float4 position : SV_Target3;
};

PixelOutputType DeferredPixelShader(PixelInputType input) : SV_TARGET
{
	PixelOutputType output;

	float4 albedoValues = gAlbedo.Sample(samAnisotropic, input.tex);
	if (albedoValues.a < 0.15f)
	{
		discard;
	}
	else
	{
		float4 roughnessValues = gRoughness.Sample(samAnisotropic, input.tex);
		float4 metallicValues = gMetallic.Sample(samAnisotropic, input.tex);

		output.albedo = float4(albedoValues.rgb, 1.0f);
		float depth = input.position.z / input.position.w;

		//calculate normal
		float3 normalMap = gNormalMap.Sample(samAnisotropic, input.tex).rgb;
		float3 normal = CalcNormal(normalMap, input.normalW, input.tangentW);
		//pack tex U as alpha channel of normal
		output.normal = float4(normal, input.tex.r);
		
		//pack tex V as alpha channel of materialProp
		output.materialProp = float4(roughnessValues.r, metallicValues.r, depth, input.tex.g);
		output.position = float4(input.positionW, 1.0f);
	}

	return output;
}