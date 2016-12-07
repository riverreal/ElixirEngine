//Average Illuminance

float GetAverageIlluminance(float3 illuminance)
{
	//HDTV method
	return dot(illuminance, float3(0.2126, 0.7152, 0.0722));
}

float GetExposure(float lum, float minLum, float maxLum, float exposureBias)
{
	//get exposure with Exposure Compensation (exposureBias) and constant 18% middle gray
	return 0.18/(clamp(lum, minLum, maxLum) + exposureBias);
}