#include "BRDF.hlsli"

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

Texture2D gDiffuseMap;
TextureCube gIrradiance;
TextureCube gEnvMap;
SamplerState samAnisotropic;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex : TEXCOORD;
};

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	
	input.normalW = normalize(input.normalW);

	float3 toEye = gEyePosW - input.positionW;
	
	float distToEye = length(toEye);
	toEye /= distToEye; //normalize

	float3 refVec = reflect(-toEye, input.normalW);
	float mipIndex = gMaterial.Properties.b * gMaterial.Properties.b * 8.0f;

	float4 texColor = float4(1, 1, 1, 1);
	float3 irradiance = float3(1, 1, 1);
	float3 envMap = float3(1, 1, 1);
	texColor = gDiffuseMap.Sample(samAnisotropic, input.tex);
	irradiance = gIrradiance.Sample(samAnisotropic, input.normalW);
	envMap = gEnvMap.SampleLevel(samAnisotropic, refVec, mipIndex);
	//texColor.rgb = pow(texColor.rgb, 2.2f);
	//envMap = pow(envMap, 2.2f);
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]
	/*
	float4 A = 0, D = 0, S = 0;
	
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
	float4 litColor =  texColor * (ambient + diffuse) + specular;
		
	*/

	float4 litColor;

	float lightIntensity = 1.0f;
	float att = 0.4;
	float ambientIntensity = 0.8f;
	float refAtt = 1.0f - gMaterial.Properties.b;
	float reflectionIntensity = gMaterial.Properties.r * refAtt;
	if (gMaterial.Properties.g >= 1.0f)
	{
		reflectionIntensity = 1.0f;
	}

	float3 light = ComputeLight(gMaterial, gDirLight, input.normalW, toEye, texColor.rgb);
	float3 albedo = texColor.rgb - texColor.rgb * gMaterial.Properties.g; //metallic
	float3 realSpec = lerp(0.03f, texColor.rgb, gMaterial.Properties.g);
	float3 halfVector = normalize(-gDirLight.Direction + toEye);
	float3 envFresnel = SchlickFresRough(input.normalW, toEye, realSpec, gMaterial.Properties.b * gMaterial.Properties.b);
	float3 VoH = saturate(dot(toEye, halfVector));
	float3 envFres = F_Schlick_Roughness(realSpec, gMaterial.Properties.b, VoH);

	//environment fresnel removed
	litColor.rgb = att * albedo * irradiance * ambientIntensity + light * lightIntensity + envMap*envFres * reflectionIntensity;

	//fog
	if(gFog.Enabled == true)
	{
		float fogLerp = saturate((distToEye - gFog.FogStart) / gFog.FogRange );
		litColor = lerp(litColor, gFog.FogColor, fogLerp);
	}

	litColor.a = gMaterial.Diffuse.a * texColor.a;
	
	return litColor;
	

	//return gDiffuseMap.Sample(samAnisotropic, input.positionW);
}