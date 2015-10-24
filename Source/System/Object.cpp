#include "Object.h"

using namespace DirectX;

Object::Object()
	:m_rotation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_scale(XMFLOAT3(0.0f, 0.0f, 0.0f))
{

}

Object::Object(bool staticObject)
{

}

Object::~Object()
{

}

void Object::SetPosition(XMFLOAT3 position)
{
	m_position = position;
}

void Object::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

void Object::SetPostiionX(float x)
{
	m_position.x = x;
}

void Object::SetPositionY(float y)
{
	m_position.y = y;
}

void Object::SetPositionZ(float z)
{
	m_position.z = z;
}

XMFLOAT3 Object::GetPosition()
{
	return m_position;
}

float Object::GetPositionX()
{
	return m_position.x;
}

float Object::GetPositionY()
{
	return m_position.y;
}

float Object::GetPositionZ()
{
	return m_position.z;
}

void Object::SetRotation(XMFLOAT3 rotation)
{
	m_rotation = rotation;
}

void Object::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x, y, z);
}

void Object::SetRotationX(float x)
{
	m_rotation.x = x;
}

void Object::SetRotationY(float y)
{
	m_rotation.y = y;
}

void Object::SetRotationZ(float z)
{
	m_rotation.z = z;
}

XMFLOAT3 Object::GetRotation()
{
	return m_rotation;
}

float Object::GetRotationX()
{
	return m_rotation.x;
}

float Object::GetRotationY()
{
	return m_rotation.y;
}

float Object::GetRotationZ()
{
	return m_rotation.z;
}

void Object::SetScale(XMFLOAT3 scale)
{
	m_scale = scale;
}

void Object::SetScale(float x, float y, float z)
{
	m_scale = XMFLOAT3(x, y, z);
}

void Object::SetScaleX(float x)
{
	m_scale.x = x;
}

void Object::SetScaleY(float y)
{
	m_scale.y = y;
}

void Object::SetScaleZ(float z)
{
	m_scale.z = z;
}

XMFLOAT3 Object::GetScale()
{
	return m_scale;
}

float Object::GetScaleX()
{
	return m_scale.x;
}

float Object::GetScaleY()
{
	return m_scale.y;
}

float Object::GetScaleZ()
{
	return m_scale.z;
}

void Object::SetOffset(offsetData offset)
{
	m_offset = offset;
}

offsetData Object::GetOffset()
{
	return m_offset;
}

XMMATRIX Object::GetWorldMatrix()
{
	return m_world;
}

void Object::Update()
{
	XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	XMMATRIX rotation = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(m_rotation.y)), 
		XMMatrixRotationX(XMConvertToRadians(m_rotation.x))), XMMatrixRotationZ(XMConvertToRadians(m_rotation.z)));
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	m_world = XMMatrixMultiply(XMMatrixMultiply(scaling, rotation), translation);
}

void Object::Render()
{

}