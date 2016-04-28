#define PI 3.14159265359f

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Properties; //r = reflectance, g = metallic, b = roughness
};

struct Fog
{
	bool Enabled;
	float FogStart;
	float FogRange;
	float4 FogColor;
};

//Distribution function GGX/Throwbridge-Reitz
float GGXDist(float NdotH, float alpha)
{
	/*
	float NoH = dot(n, h);
	float alpha2 = alpha * alpha;
	float NoH2 = NoH * NoH;
	float den = NoH2 * alpha2 + (1 - NoH2);
	return (chiGGX(NoH) * alpha2) / (3.1416 * den * den);
	*/
	//Reparameterization of alpha [Disney]
	float alpha2 = alpha * alpha;
	float NdotH2 = NdotH * NdotH;

	float denominator = NdotH2 * (alpha2 - 1.0f) + 1.0f;
	denominator *= denominator;
	denominator *= PI;

	return alpha2 / denominator;
}

//Smith Schlick GGX
float Geo_SchlickGGX(float NdotV, float NdotL, float alpha)
{
	//Crytek k = ((0.8 + a/2)**2) /2
	
	float k = (0.8 + 0.5f * alpha);
	k = k * k;
	k = 0.5f * k;
	
	//schlick k = a/2
	//float k = alpha * 0.5f;

	//Disney method k = alpha / 2, but alpha = (alpha + 1) / 2
	/*
	float k = alpha + 1;
	k = k * k;
	k = k * 0.125f;
	*/

	//Smith
	// G(l, v, h) = G1(l)G1(v)

	float GV = NdotV / (NdotV * (1 - k) + k);
	float GL = NdotL / (NdotL * (1 - k) + k);

	return GV * GL;
}

float3 SchlickFres(float h, float v, float3 specColor)
{
	float VdotH = saturate(dot(v, h));

	//Standard
	//(F0 + (1.0f - F0) * pow(1.0f - VdotH, 5));

	//return (specColor + (1.0f - specColor) * pow(1.0f - saturate(dot(v, h)), 5));

	float fresnel = pow(1.0f - VdotH, 5);
	fresnel *= (1.0f - specColor);
	fresnel += specColor;

	return fresnel;
	//float Fc = pow((1 - VdotH), 5);
	//return saturate(50.0 * specColor.g) * Fc + (1 - Fc) * specColor;

	//UE4 modificaction
	//specColor + (1.0f - specColor) * pow(2, ((-5.55473*VdotH-6.98316)VdotH));

	//return specColor + (1.0f - specColor) * pow(2, ((-5.55473*VdotH - 6.98316)*VdotH));
}

float3 SchlickFresRough(float h, float v, float3 specColor, float a)
{
	float VdotH = saturate(dot(v, h));
	//attenuation by glossiness (1 - roughness)
	return (specColor + (min(1.0f - a, specColor) - specColor) * pow((1.0f - VdotH), 5));
}

float CelShadingFunc(float factor)
{
	float newFactor = 0.0f;

	if (factor <= 0.0f)
	{
		newFactor = 0.1;
	}
	else if (factor > 0.0f && factor <= 0.2f)
	{
		newFactor = 0.4;
	}
	else if (factor > 0.2f && factor <= 1.0f)
	{
		newFactor = 1.0f;
	}

	return newFactor;
}

float3 Diffuse(float3 albedo)
{
	return albedo * 1 / PI;
}

float CookTorranceSpecularFactor(float3 normal, float3 viewer, float metallic, float roughness, DirectionalLight dirL, float3 albedo)
{
	float distribution;
	float3 light = -dirL.Direction;
	float3 halfVector = normalize(light + viewer);

	float ior = 1.6;

	float NdotL = saturate(dot(normal, light));
	float NdotH = saturate(dot(normal, halfVector));
	float NdotV = saturate(dot(normal, viewer));
	float VdotH = saturate(dot(viewer, halfVector));
	float r_sq = max(0.001f, roughness * roughness);
	/*

	//Cook-Torrance Geo
	float geo_numerator = 2.0f * NdotH;
	float geo_denominator = VdotH;

	float geo_b = (geo_numerator * NdotV) / geo_denominator;
	float geo_c = (geo_numerator * NdotL) / geo_denominator;
	float geo = min(1.0f, min(geo_b, geo_c));

	//Gaussian roughness
	//float c = 1.0f;
	//float alpha = acos(dot(normal, halfVector));
	//distribution = c * exp(-(alpha / r_sq));

	//beckmann roughness
	float r1 = 1.0 / (4.0 * r_sq * pow(NdotH, 4.0));
	float r2 = (NdotH * NdotH - 1.0) / (r_sq * NdotH * NdotH);
	distribution = r1 * exp(r2);

	//Schlick-GGX Geo
	float k = roughness / 2;
	float geo_denominator = (NdotV * (1 - k)) + k;
	float geo = NdotV / geo_denominator;
	*/
	//metallic = clamp(metallic, 0, 1);
	//float3 f0 = abs((1.0 - ior) / (1.0 + ior));
	//f0 = f0 * f0;
	//f0 = lerp(f0, albedo, metallic);
	//real specular color 
	float3 realSpec = lerp(0.03f, albedo, metallic);

	//float3 specColor = lerp(0.03f, albedo, metallic);
	float3 fresnel = SchlickFres(viewer, halfVector, realSpec);
	//float fresnel = pow(1.0f - VdotH, 5);
	//fresnel *= (1.0f - 1.0f);
	//fresnel += 1.0f;

	float geo = Geo_SchlickGGX(NdotV, NdotL, r_sq);
	distribution = GGXDist(NdotH, r_sq);

	float3 Rs_numerator = (fresnel * geo * distribution);
	float Rs_denominator = 4.0f * (NdotV * NdotL) + 0.0001f; //Added small fraction to denominator to prevent light aliasing on metals
	float3 Rs = Rs_numerator / Rs_denominator;

	return Rs;
}

float3 ComputeLight(Material mat, DirectionalLight L,
	float3 normal, float3 toEye, float3 albedo)
{
	float3 resultLight = float3(0.0f, 0.0f, 0.0f);
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.Direction;

	//-----------Lambert-------------
	float NdotL = saturate(dot(normal, lightVec));
	// Flatten to avoid dynamic branching.
	float specular = CookTorranceSpecularFactor(normal, toEye, mat.Properties.g, mat.Properties.b, L, albedo);

	float3 diffuse = Diffuse(albedo);

	resultLight = L.Diffuse.rgb * NdotL * (diffuse * (1.0f - specular) + specular);
	//resultLight = specular;
	//resultLight = NdotL * diffuse + specular;

	return resultLight;
}

void ComputeDirectionalLight(Material mat, DirectionalLight L,
	float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.Direction;

	// Add ambient term.
	ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float specFactor;
		
		float3 h = normalize(lightVec + toEye);
		float NdotH = max(0.0f, dot(normal, h));
		specFactor = pow(NdotH, mat.Specular.w);
		
		/*
		float3 reflexion = normalize(2.0f * normal * dot(normal, lightVec) - lightVec);
		float RdotV = max(dot(reflexion, toEye), 0.0f);
		specFactor = pow(RdotV, mat.Specular.w);
		*/

		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}
	
}

void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;

	// The distance from surface to light.
	float d = length(lightVec);

	// Range test.
	if (d > L.Range)
		return;

	// Normalize the light vector.
	lightVec /= d;

	// Ambient term.
	ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}

	// Attenuate
	//float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));
	float att = clamp(1.0f - d*d / (L.Range*L.Range), 0.0f, 1.0f);

	diffuse *= att;
	spec *= att;
}

void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;

	// The distance from surface to light.
	float d = length(lightVec);

	// Range test.
	if (d > L.Range)
		return;

	// Normalize the light vector.
	lightVec /= d;

	// Ambient term.
	ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}

	// Scale by spotlight factor and attenuate.
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);

	// Scale by spotlight factor and attenuate.
	float att = spot / dot(L.Att, float3(1.0f, d, d*d));

	ambient *= spot;
	diffuse *= att;
	spec *= att;
}

