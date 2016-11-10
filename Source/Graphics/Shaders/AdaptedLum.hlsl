#include "EyeAdaptation.hlsli"

Texture2D gAverageIllum : register(t0);
Texture2D gOldIllum : register(t1);

SamplerState samplerLinear;

cbuffer adaptationBuffer : register(b0)
{
	float gDt;
	uint gFirstPass;
	float2 pad;
};

struct VertexOut
{
	float4 PosL : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 AdaptedLum(VertexOut input) : SV_TARGET
{
	float4 newIllum = gAverageIllum.Sample(samplerLinear, input.tex);

	if (gFirstPass)
	{
		float avrg = GetAverageIlluminance(newIllum.rgb);
		return float4(avrg, avrg, avrg, 1.0f);
	}

	float Tau = 1.25f;
	
	//oldIllum is processed
	float oldIllum = gOldIllum.Sample(samplerLinear, input.tex).r;

	//process new
	float averageIllum = newIllum.r;
	float delta = 0.001f;
	//log luminance
	//averageIllum = log(delta + averageIllum);

	//Slowly adapt to new luminance

	float adaptionSpeed = 3.0f;
	if (oldIllum > averageIllum)
	{
		adaptionSpeed = 5.0f;
	}

	//lerp(oldIllum.r, 0.5/averageIllum, 3 * gDt);
	float adaptedIll = oldIllum + ((averageIllum - oldIllum) * (1.0f - exp2(-adaptionSpeed * gDt)));

	newIllum = float4(adaptedIll, adaptedIll, adaptedIll, 1.0f);

	return newIllum;
}