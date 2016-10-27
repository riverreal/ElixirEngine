float4 FilmicTonemap(float4 colorIn)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;

	float exposureBias = 2.0f;

	float3 x = exposureBias * colorIn.rgb;

	float3 curr = ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;

	float3 whiteScale = 1.0f / ((W*(A*W + C*B) + D*E) / (W*(A*W + B) + D*F)) - E / F;
	float3 color = curr * whiteScale;

	float3 retColor = pow(color.rgb, 1 / 2.2);

	return float4(retColor, colorIn.w);
}
