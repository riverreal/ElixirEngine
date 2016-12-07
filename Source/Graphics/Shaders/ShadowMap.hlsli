static const float SMAP_SIZE = 2048.0f;
static const float SMAP_TEXEL = 1.0f / SMAP_SIZE;
static const float SMAP_DEPTH_BIAS = 0.001f;

float CalcShadowFactor(SamplerComparisonState samShadow, Texture2D shadowMap,
	float4 shadowPos)
{
	//shadowPos.xyz /= shadowPos.w;

	float depth = shadowPos.z - SMAP_DEPTH_BIAS;

	const float dx = SMAP_TEXEL;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, dx), float2(0.0f, dx), float2(dx, dx)
	};

	//4-tap PCF each sample
	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += shadowMap.SampleCmpLevelZero(samShadow, shadowPos.xy + offsets[i], depth).r;
	}

	//average
	return percentLit /= 9.0f;
}