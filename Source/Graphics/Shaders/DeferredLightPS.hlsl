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
cbuffer LightBuffer : register(b0)
{
	PBRDirectionalLight gDirLight;
	PBRPointLight gPointLight;
	PBRSpotLight gSpotLight;
	float pad;
};

cbuffer cdPerObject : register(b1)
{
	float3 gEyePosW;
};

cbuffer FogBuffer : register(b2)
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

	if (depth < 0.001f)
	{
		outputColor = albedo;
		return outputColor;
	}

	//Gamma correction
	albedo.rgb = pow(albedo.rgb, 2.2f);

	//screen space normal
	normal = 2.0f * normal - 1.0f;
	normal = normalize(normal);
	
	//Variables------------------------------------------------------------------------
	
	float roughness = clamp(material.r, 0.0, 1.0);
	float metallic = clamp(material.g, 0.0, 1.0);
	float2 texCoord = material.ba;
	float3 toEye = gEyePosW - position.rgb;
	float distToEye = length(toEye);
	toEye /= distToEye;
	float3 refVec = reflect(-toEye, normal.rgb);
	float mipIndex = roughness * roughness * 11.0f;
	
	
	//Data for IBL --------------------------------------------------------------------
	float3 irradiance = irradianceTexture.Sample(SampleTypePoint, normal.rgb);
	float3 envMap = envMapTexture.SampleLevel(SampleTypePoint, refVec, mipIndex);
	//gamma correction
	envMap = pow(envMap, 2.2f);

	float3 realAlbedo = albedo - albedo * metallic;
	float3 realSpecularColor = lerp(0.03f, albedo, metallic);

	//Per Light-----------------------------------------------------------------------------------------------------------------------

	float lightIntensity = gDirLight.LightIntensity.x;
	float reflectionIntensity = 1.0;
	float ambientIntensity = gDirLight.LightIntensity.y;

	//Env factor----------------------------------------------------------------------------------------------------------------------
	float3 envFresnel = (realSpecularColor + (max(1.0f - roughness*roughness, realSpecularColor) - realSpecularColor) * pow(1 - saturate(dot(normal, toEye)), 5));
	float3 envFactor = envFresnel * envMap * reflectionIntensity;

	//Directional Light---------------------------------------------------------------------------------------------------------------
	float3 lightDir = gDirLight.Direction;
	float3 lightColor = gDirLight.LightColor.rgb;
	float NoL = saturate(dot(normal, -lightDir));
	float3 halfVector = normalize(-lightDir + toEye);
	float VoH = saturate(dot(toEye, halfVector));

	float3 diff = lightColor * realAlbedo * NoL;
	float3 spec = CookTorranceSpecFactor(normal, toEye, metallic, roughness, lightDir, albedo)  * lightColor * NoL * lightIntensity;

	//Point Lights--------------------------------------------------------------------------------------------------------------------
	
	lightDir = -normalize(gPointLight.Position.rgb - position.rgb);
	lightColor = gPointLight.LightColor.rgb;
	NoL = saturate(dot(normal, -lightDir));
	halfVector = normalize(-lightDir + toEye);
	VoH = saturate(dot(toEye, halfVector));

	float lightDist = length(gPointLight.Position.rgb - position.rgb);
	float attenuation = clamp(1.0f - ((lightDist * lightDist)  / (gPointLight.Range*gPointLight.Range)), 0.0f, 1.0f);
	attenuation = attenuation * attenuation;
	//lighting calculation
	diff += lightColor * realAlbedo * NoL * attenuation;
	spec += CookTorranceSpecFactor(normal, toEye, metallic, roughness, lightDir, albedo) * attenuation * lightColor * NoL;
	
	//Final Color--------------------------------------------------------------------------------------------------------------------

	float3 specFactor = spec;
	float3 diffuseFactor = diff;
	float3 ambientFactor = realAlbedo * irradiance * ambientIntensity;

	outputColor = float4( specFactor + diffuseFactor + ambientFactor + envFactor, 1.0f);
	
	if (gFog.Enabled == true)
	{
		float fogLerp = saturate((distToEye - gFog.FogStart) / gFog.FogRange);
		outputColor = lerp(outputColor, gFog.FogColor, fogLerp);
	}

	return outputColor;// float4(specFactor, 1.0f);
}