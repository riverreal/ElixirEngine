#include "BRDF.hlsli"

//each GBuffer texture
Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D materialTexture : register(t2); //x = roughness, y = metallic
Texture2D positionTexture : register(t3);

//IBL texture
TextureCube irradianceTexture : register(t4);
TextureCube envMapTexture : register(t5);

//sampler states
SamplerState SampleTypePoint : register(s0);

//globals
cbuffer LightBuffer
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	float3 gEyePosW;
	float pad;
};

cbuffer FogBuffer
{
	Fog gFog;
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 DeferredLightPS(PixelInputType input) : SV_TARGET
{
	float4 outputColor;
	
	//Retrieve Data from GBuffer------------------------------------------------------
	float4 albedo;
	float4 normalTexR;
	float3 normal;
	float4 material;
	float4 position;
	float2 texTrans;
	float depth;
	albedo = albedoTexture.Sample(SampleTypePoint, input.tex);
	normalTexR = normalTexture.Sample(SampleTypePoint, input.tex);
	material = materialTexture.Sample(SampleTypePoint, input.tex);
	position = positionTexture.Sample(SampleTypePoint, input.tex);
	normal = normalTexR.rgb;
	depth = material.b;
	//Gamma correction
	albedo.rgb = pow(albedo.rgb, 2.2f);

	//screen space normal
	normal = 2.0f * normal - 1.0f;
	normal = normalize(normal);

	//Variables------------------------------------------------------------------------
	float lightIntensity = 8.0f;
	float reflectionIntensity = 1.0;
	float ambientIntensity = 0.8f;
	float roughness = clamp(material.r, 0.0, 1.0);
	float metallic = clamp(material.g, 0.0, 1.0);
	float2 texCoord = material.ba;
	float3 toEye = gEyePosW - position.rgb;
	float distToEye = length(toEye);
	toEye /= distToEye;
	float3 refVec = reflect(-toEye, normal.rgb);
	float mipIndex = roughness * roughness * 11.0f;
	float3 lightColor = gDirLight.Diffuse.rgb;
	float NoL = saturate(dot(normal, -gDirLight.Direction));
	float3 halfVector = normalize(-gDirLight.Direction + toEye);
	float VoH = saturate(dot(toEye, halfVector));
	//Data for IBL --------------------------------------------------------------------
	float3 irradiance = irradianceTexture.Sample(SampleTypePoint, normal.rgb);
	float3 envMap = envMapTexture.SampleLevel(SampleTypePoint, refVec, mipIndex);
	//gamma correction
	envMap = pow(envMap, 2.2f);

	float3 realAlbedo = albedo - albedo * metallic;
	float3 realSpecularColor = lerp(0.03f, albedo, metallic);

	//lighting calculation
	float3 diff = Diffuse(realAlbedo);
	float3 spec = CookTorranceSpecFactor(normal, toEye, metallic, roughness, gDirLight, albedo);
	float3 light = lightColor * NoL * (diff * (1.0f - spec) + spec);
	float3 envFresnel = (realSpecularColor + (max(1.0f - roughness*roughness, realSpecularColor) - realSpecularColor) * pow(1 - saturate(dot(normal, toEye)), 5));

	if (depth < 0.001f)
	{
		outputColor = albedo;
	}
	else
	{
		outputColor = float4( lightIntensity * light + envFresnel * envMap * reflectionIntensity + realAlbedo * irradiance * ambientIntensity, 1.0f);
	}
	
	
	return outputColor;
}