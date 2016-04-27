//globals
//each buffer texture
Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
//---- other buffers

//sampler states
SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer
{
	float3 lightDirection;
	float padding;
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 DeferredLightPS(PixelInputType input) : SV_TARGET
{
	float4 albedo;
	float4 normals;
	float3 lightDir;
	float lightIntensity;
	float4 outputColor;

	albedo = albedoTexture.Sample(SampleTypePoint, input.tex);
	normals = normalTexture.Sample(SampleTypePoint, input.tex);

	lightDir = -lightDirection;

	lightIntensity = saturate(dot(normals.xyz, lightDir));

	outputColor = saturate(albedo * lightIntensity);

	return outputColor;
}