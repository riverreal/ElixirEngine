#include "BRDF.hlsli"
#include "ShadowMap.hlsli"

#define MAX_POINT_LIGHT 50
#define MAX_SPOT_LIGHT 20

//each GBuffer texture
Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1); //w = U
Texture2D materialTexture : register(t2); //x = roughness, y = metallic, z = Depth, w = V
Texture2D positionTexture : register(t3);

//IBL texture
TextureCube irradianceTexture : register(t4);
TextureCube envMapTexture : register(t5);

//Shadow map
Texture2D shadowMapTexture : register(t6);

//sampler states
SamplerState SampleTypePoint : register(s0);

SamplerState samShadow : register(s1);

//globals

cbuffer ShadowMapBuffer : register(b0)
{
	matrix gLightProj;
	matrix gLightView;
	matrix gViewMatrix;
	float pad;
};

cbuffer LightBuffer : register(b1)
{
	PBRDirectionalLight gDirLight;
	U32 gNumPointLights;
	float3 pad1;
	U32 gNumSpotLights;
	float3 pad2;
	PBRPointLight gPointLight[MAX_POINT_LIGHT];
	PBRSpotLight gSpotLight[MAX_SPOT_LIGHT];
};

cbuffer cdPerObject : register(b2)
{
	float3 gEyePosW;
};

cbuffer FogBuffer : register(b3)
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

	//world space normal
	normal = 2.0f * normal - 1.0f;
	normal = clamp(normal, -1.0f, 1.0f);
	normal = normalize(normal);
	//Variables------------------------------------------------------------------------
	
	float roughness = saturate(material.r);
	float metallic = saturate(material.g);
	float2 texCoord = float2(normalTexR.a, material.a);
	float3 toEye = gEyePosW - position.rgb;
	float distToEye = length(toEye);
	toEye /= distToEye;
	float3 refVec = reflect(-toEye, normal);
	float mipIndex = roughness * roughness * 11.0f;
	
	//Data for IBL --------------------------------------------------------------------
	float3 irradiance = irradianceTexture.Sample(SampleTypePoint, normal);
	float3 envMap = envMapTexture.SampleLevel(SampleTypePoint, refVec, mipIndex);
	//gamma correction
	envMap = pow(envMap, 2.2f);

	float3 realAlbedo = albedo - albedo * metallic;
	float3 realSpecularColor = lerp(0.03f, albedo, metallic);

	//Per Light-----------------------------------------------------------------------------------------------------------------------

	float lightIntensity = gDirLight.LightIntensity.x;
	float reflectionIntensity = gDirLight.LightIntensity.y;
	float ambientIntensity = gDirLight.LightIntensity.y;

	//Env factor----------------------------------------------------------------------------------------------------------------------
	float3 envFresnel = (realSpecularColor + (max(1.0f - roughness*roughness, realSpecularColor) - realSpecularColor) * pow(1 - saturate(dot(normal, toEye)), 5));
	float3 envFactor = envFresnel * envMap * reflectionIntensity;

	//Shadow--------------------------------------------------------------------------------------------------------------------------
	//calculate shadow contribution
	
	float4 lightVP = mul(float4(position.xyz, 1.0f), gLightProj);

	float4 shadowCoord = mul(lightVP, gLightView);
	shadowCoord.xyz /= shadowCoord.w;

	//shadowCoord.xyz = shadowCoord.xyz * float3(0.5f, 0.5f, 0.5f) + float3(0.5f, 0.5f, 0.5f);
	shadowCoord.x = shadowCoord.x / 2.0f + 0.5f;
	shadowCoord.y = -shadowCoord.y / 2.0f + 0.5f;

	float shadowContrib = 1.0;

	if ((saturate(shadowCoord.x) == shadowCoord.x) && (saturate(shadowCoord.y) == shadowCoord.y))
	{
		float a = shadowMapTexture.Sample(samShadow, shadowCoord.xy).r;
		float b = shadowCoord.z - 0.001f;
		
		shadowContrib = a > b ? 1.0f : 0.0f;
	}
	
	/*
	shadowContrib = 1.0f;// CalcShadowFactor(samShadow, shadowMapTexture, shadowCoord);

	const float dx = SMAP_TEXEL;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, dx), float2(0.0f, dx), float2(dx, dx)
	};

	return float4(b, b, b, 1.0f);

	//4-tap PCF each sample
	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += shadowMapTexture.SampleCmpLevelZero(samShadow, shadowCoord.xy + offsets[i], b).r;
		
	}

	percentLit /= 9.0f;
	shadowContrib = percentLit;
	*/

	//Directional Light---------------------------------------------------------------------------------------------------------------
	float3 diff = 0.0f;
	float3 spec = 0.0f;
	float3 lightDir = gDirLight.Direction;
	float3 lightColor = gDirLight.LightColor.rgb;
	float NoL = saturate(dot(normal, -lightDir));
	float3 halfVector = normalize(-lightDir + toEye);
	float VoH = saturate(dot(toEye, halfVector));

	if (shadowContrib != 0.0f)
	{
		
		
		diff = lightColor * realAlbedo * NoL * irradiance * lightIntensity;
		spec = GGXSpecFactor(normal, toEye, metallic, roughness, lightDir, albedo)  * lightColor * NoL * lightIntensity;
	}
	
	diff *= shadowContrib;
	spec *= shadowContrib;

	//Point Lights--------------------------------------------------------------------------------------------------------------------
	
	uint numPoint = gNumSpotLights;
	numPoint = clamp(numPoint, 0, MAX_POINT_LIGHT);

	for (uint i = 0; i < numPoint; ++i)
	{
		lightDir = -normalize(gPointLight[i].Position.rgb - position.rgb);
		lightColor = gPointLight[i].LightColor.rgb;
		NoL = saturate(dot(normal, -lightDir));
		halfVector = normalize(-lightDir + toEye);
		VoH = saturate(dot(toEye, halfVector));

		float lightDist = length(gPointLight[i].Position.rgb - position.rgb);
		float attenuation = clamp(1.0f - ((lightDist * lightDist) / (gPointLight[i].Range*gPointLight[i].Range)), 0.0f, 1.0f);
		attenuation = attenuation * attenuation;
		//lighting calculation
		diff += lightColor * realAlbedo * NoL * attenuation * gPointLight[i].LightColor.a;
		spec += GGXSpecFactor(normal, toEye, metallic, roughness, lightDir, albedo) * attenuation * lightColor * NoL * gPointLight[i].LightColor.a;
	}
	
	//Spot Lights--------------------------------------------------------------------------------------------------------------------
	
	uint numSpot = gNumSpotLights;
	numSpot = clamp(numSpot, 0, MAX_SPOT_LIGHT);

	for (uint i = 0; i < numSpot; ++i)
	{
		
		//lightDir = -normalize(gSpotLight[i].Position.rgb - position.rgb);
		lightDir = gSpotLight[i].Direction;
		float3 lightVec = gSpotLight[i].Position.rgb - position.rgb;
		float d = length(lightVec);

		//Skip iteration if light is out of range
		if (d > gSpotLight[i].Range)
		{
			continue;
		}

		lightVec /= d;
		lightColor = gSpotLight[i].LightColor.rgb;
		NoL = saturate(dot(normal, -lightDir));
		halfVector = normalize(-lightDir + toEye);
		VoH = saturate(dot(toEye, halfVector));

		//angle of the spotlight
		float spot = pow(max(dot(-lightVec, lightDir), 0.0f), gSpotLight[i].Spot);
		float att = spot / dot(gSpotLight[i].Att, float3(1.0f, d, d * d));

		diff += gSpotLight[i].LightColor.rgb * realAlbedo * NoL * att * gSpotLight[i].LightColor.a;
		spec += GGXSpecFactor(normal, toEye, metallic, roughness, lightDir, albedo) * att * gSpotLight[i].LightColor * NoL * gSpotLight[i].LightColor.a;
	}
	
	//Final Color--------------------------------------------------------------------------------------------------------------------

	float3 specFactor = spec;
	float3 diffuseFactor = diff * (1 - metallic);
	float3 ambientFactor = realAlbedo * ambientIntensity;
	
	outputColor = float4(ambientFactor + envFactor + diffuseFactor + specFactor, 1.0f);
	
	if (gFog.Enabled == true)
	{
		float fogLerp = saturate((distToEye - gFog.FogStart) / gFog.FogRange);
		outputColor = lerp(outputColor, gFog.FogColor, fogLerp);
	}
	
	return outputColor;//float4(normal, 1.0f);
}