/*------------------------------------------------------
	Radix Engine 2016
	Bidirectional Reflectance Distribution Function

	**************Cook-Torrance BRDF*****************
-------------------------------------------------------*/

#include "LightHelper.hlsli"

#define PI 3.14159265359f

//Distribution function
//GGX --->>            0
//Blinn --->>          1 //not available yet
#define SPECULAR_D_GGX 1

//Geometry term
#define SPECULAR_G_SCHLICK 0
#define SPECULAR_G_GGX 1

//Fresnel
//None --->>           0
//Schlick --->>        1
#define SPECULAR_F_NONE 0
#define SPECULAR_F_SCHLICK 1

//K term for schlick geometry term
//Default Schlick --->>    0
//Crytek model --->>       1
//Disney --->>             2   <--- only for analytic light sources
#define K_MODEL_SCHLICK 1
#define K_MODEL_CRYTEK 0
#define K_MODEL_DISNEY 0

//Diffuse functions
float3 DiffuseLambert(float3 diffuseColor)
{
	return diffuseColor * (1 / PI);
}

float chiGGX(float v)
{
	return v > 0 ? 1 : 0;
}

//Distribution functions
//GGX/Throwbridge-Reitz
float D_GGX(float roughness, float NoH)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NoH2 = NoH * NoH;
	float denom = NoH2 * (a2 - 1.0f) + 1.0f;

	return a2 / (PI * denom * denom);
}

//Geometry term functions

float G_Smith_Schlick(float roughness, float NoV, float NoL)
{
	float a = roughness * roughness;
	float k;
	
#if K_MODEL_SCHLICK
	k = a * 0.5f;
#elif K_MODEL_CRYTEK
	k = (0.8f + 0.5f * a);
	k = k * k;
	k = 0.5f * k;
#elif K_MODEL_DISNEY
	k = a + 1;
	k = k * k;
	k = k * 0.125f;
#endif

	//k = a * 0.5f;
	float GV = NoV / (NoV * (1 - k) + k);
	float GL = NoL / (NoL * (1 - k) + k);

	return GV * GL;
}

float G_Smith_GGX(float roughness, float NoV, float NoL)
{
	float a = roughness * roughness;
	float GV = NoL * (NoV * (1 - a) + a);
	float GL = NoV * (NoL * (1 - a) + a);
	return 0.5 * rcp(GV + GL);
}

//Fresnel functions
float3 F_None(float3 SpecularColor)
{
	return SpecularColor;
}

float3 F_Schlick(float3 SpecularColor, float VoH)
{
	float Fc = pow((1 - VoH), 5);

	return saturate(50.0f * SpecularColor.g) * Fc + (1 - Fc) * SpecularColor;
}

//Reflectance at normal incidence
//Reference: codinglabs.net
float3 RefAtNormalIncidence(float3 albedo, float metallic, float reflectivity)
{
	float ior = 1 + reflectivity;
	float3 F0 = abs((1.0 - ior) / (1.0 + ior));
	F0 = F0 * F0;
	F0 = lerp(F0, albedo, metallic);
	return F0;
}

//Epic uses this
float3 F_Schlick_Gau_Ver(float VoH, float3 F0)
{
	//normal way
	//return F0 + (1 - F0) * pow((1 - VoH), 5);

	//Spherical Gaussian Approximation
	//Reference: Seb. Lagarde's Blog (seblagarde.wordpress.com)
	return F0 + (1 - F0) * exp2((-5.55473 * VoH - 6.98316) * VoH);
}

float3 F_Schlick_With_F0(float VoH, float3 albedo, float metallic, float reflectivity)
{
	float3 F0 = RefAtNormalIncidence(albedo, metallic, reflectivity);
	return F_Schlick_Gau_Ver(VoH, F0);
}

float3 F_Schlick_Roughness(float3 SpecularColor, float roughness, float VoH)
{
	float a = roughness * roughness;
	return (SpecularColor + (max(1.0f - a, SpecularColor) - SpecularColor) * pow(1 - VoH, 5));
}

//Cook Torrance model
float CookTorranceSpecFactor(float3 normal, float3 viewer, float metallic, float roughness, float3 lightDir, float3 albedo)
{
	float3 light = -lightDir;
	float3 halfVector = normalize(light + viewer);

	float NoL = saturate(dot(normal, light));
	float NoH = saturate(dot(normal, halfVector));
	float NoV = saturate(dot(normal, viewer));
	float VoH = saturate(dot(viewer, halfVector));
	float LoH = saturate(dot(light, halfVector));

	float3 realSpec = lerp(0.03f, albedo, metallic);

	//Fresnel
	float3 fresnel;
//#if SPECULAR_F_NONE
	//fresnel = F_None(realSpec);
//#elif SPECULAR_F_SCHLICK
	fresnel = F_Schlick(realSpec, NoV);
//#endif
	//G term
	float geometry;
//#if SPECULAR_G_SCHLICK
	geometry = G_Smith_Schlick(roughness, NoV, NoL);
//#elif SPECULAR_G_GGX
	//geometry = G_Smith_GGX(roughness, NoV, NoL);
//#endif

	float distribution;
//#if SPECULAR_D_GGX
	distribution = D_GGX(roughness, NoH);
//#endif

	float3 numerator = (fresnel * geometry * distribution);
	float denominator = 4.0f * (NoV * NoL) + 0.0001; //prevent light aliasing on metals

	return numerator / denominator;
}

//Improved
//Diffuse:
//DiffuseLambert

//Specular:
//D = D_GGX
//G = G_Smith_Schlick
//F = F_Schlick_With_F0

float3 GGXSpecFactor(float3 normal, float3 viewer, float metallic, float roughness, float3 lightDir, float3 albedo)
{
	float3 light = -lightDir;
	float3 halfVector = normalize(light + viewer);

	float NoL = saturate(dot(normal, light));
	float NoH = saturate(dot(normal, halfVector));
	float NoV = saturate(dot(normal, viewer));
	float VoH = saturate(dot(viewer, halfVector));
	float LoH = saturate(dot(light, halfVector));

	float3 realSpec = lerp(0.03f, albedo, metallic);

	float3 fresnel = F_Schlick_With_F0(VoH, albedo, metallic, 0.5);
	float geometry = G_Smith_Schlick(roughness, NoV, NoL);
	float distribution = D_GGX(roughness, NoH);


	float3 numerator = (fresnel * geometry * distribution);
	float denominator = 4.0f * (NoV * NoL) + 0.0001; //prevent light aliasing on metals

	return numerator / denominator;
}
