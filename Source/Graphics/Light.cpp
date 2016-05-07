#include "Light.h"

using namespace DirectX;

Light::Light()
{
	m_directionalLight = PBRDirectionalLight();
	m_directionalLight.LightColor[0] = 0.5f;
	m_directionalLight.LightColor[1] = 0.5f;
	m_directionalLight.LightColor[2] = 0.5f;
	m_directionalLight.LightColor[3] = 1.0f;
	m_directionalLight.Direction[0] = 1.0f;
	m_directionalLight.Direction[1] = -1.0f;
	m_directionalLight.Direction[2] = 0.0f;
	m_directionalLight.LightIntensity[0] = 2.0f;
	m_directionalLight.LightIntensity[1] = 1.0f;
	m_directionalLight.LightIntensity[2] = 1.0f;
	m_directionalLight.LightIntensity[3] = 1.0f;
}

Light::~Light()
{
}

void Light::CreatePointLight(PBRPointLight pointLight)
{
	m_pointLightVector.push_back(pointLight);
}

void Light::CreatePointLight(float posX, float posY, float posZ, float lightColorR, float lightColorG, float lightColorB, float range)
{
	PBRPointLight pointLight(posX, posY, posZ, lightColorR, lightColorG, lightColorB, range);
	m_pointLightVector.push_back(pointLight);
}

PBRDirectionalLight Light::GetDirectionalLight()
{
	return m_directionalLight;
}

PBRPointLight Light::GetPointLight(int index)
{
	return m_pointLightVector[index];
}

PBRSpotLight Light::GetSpotLight(int index)
{
	return m_spotLightVector[index];
}
