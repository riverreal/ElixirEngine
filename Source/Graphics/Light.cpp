#include "Light.h"

using namespace DirectX;

Light::Light()
	:m_pointLightCount(0),
	m_spotLightCount(0)
{
	m_directionalLight = PBRDirectionalLight();
	m_directionalLight.LightColor[0] = 0.3f;
	m_directionalLight.LightColor[1] = 0.3f;
	m_directionalLight.LightColor[2] = 0.3f;
	m_directionalLight.LightColor[3] = 1.0f;
	m_directionalLight.Direction[0] = 1.0f;
	m_directionalLight.Direction[1] = -1.0f;
	m_directionalLight.Direction[2] = 0.0f;
	m_directionalLight.LightIntensity[0] = 0.6f;
	m_directionalLight.LightIntensity[1] = 1.0f;
	m_directionalLight.LightIntensity[2] = 1.0f;
	m_directionalLight.LightIntensity[3] = 1.0f;
}

Light::~Light()
{
}

void Light::CreatePointLight(PBRPointLight pointLight)
{
	if (m_pointLightCount < MAX_POINT_LIGHTS)
	{
		m_pointLightVector.push_back(pointLight);
		m_pointLightCount++;
	}
}

void Light::CreatePointLight(float posX, float posY, float posZ, float lightColorR, float lightColorG, float lightColorB, float range)
{
	if (m_pointLightCount < MAX_POINT_LIGHTS)
	{
		PBRPointLight pointLight(posX, posY, posZ, lightColorR, lightColorG, lightColorB, range);
		m_pointLightVector.push_back(pointLight);
		m_pointLightCount++;
	}
}

void Light::CreateSpotLight(PBRSpotLight spotLight)
{
	if (m_spotLightCount < MAX_SPOT_LIGHTS)
	{
		m_spotLightVector.push_back(spotLight);
		m_spotLightCount++;
	}
}

void Light::CreateSpotLight(float posX, float posY, float posZ, float lightColorR, float lightColorG, float lightColorB, float range, DirectX::XMFLOAT3 dir, float spot, float constAtt, float linearAtt, float quadranticAtt)
{
	if (m_spotLightCount < MAX_SPOT_LIGHTS)
	{
		PBRSpotLight spotLight(posX, posY, posZ, lightColorR, lightColorG, lightColorB, range, spot, constAtt, linearAtt, quadranticAtt, dir.x, dir.y, dir.z);
		m_spotLightVector.push_back(spotLight);
		m_spotLightCount++;
	}
}

PBRDirectionalLight Light::GetDirectionalLight()
{
	return m_directionalLight;
}

void Light::SetDirectionalLight(PBRDirectionalLight dirL)
{
	m_directionalLight = dirL;
}

PBRPointLight Light::GetPointLight(int index)
{
	return m_pointLightVector[index];
}

PBRSpotLight Light::GetSpotLight(int index)
{
	return m_spotLightVector[index];
}

PBRPointLight * Light::GetModPointLight(int index)
{
	return &m_pointLightVector[index];
}

PBRSpotLight * Light::GetModSpotLight(int index)
{
	return &m_spotLightVector[index];
}

U32 Light::GetPointLightCount()
{
	return m_pointLightCount;
}

U32 Light::GetSpotLightCount()
{
	return m_spotLightCount;
}


