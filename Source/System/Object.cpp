#include "Object.h"

using namespace DirectX;

Object::Object()
	:m_rotation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_scale(XMFLOAT3(1.0f, 1.0f, 1.0f)),
	m_texPos(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_texSca(XMFLOAT3(1.0f, 1.0f, 1.0f)),
	m_texRot(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_texTrans(XMMatrixIdentity()),
	m_world(XMMatrixIdentity()),
	m_textureID(0),
	m_materialID(0),
	m_disabled(false),
	m_dynamic(true)
{
	Update();
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
	Update();
}

void Object::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
	Update();
}

void Object::SetPositionX(float x)
{
	m_position.x = x;
	Update();
}

void Object::SetPositionY(float y)
{
	m_position.y = y;
	Update();
}

void Object::SetPositionZ(float z)
{
	m_position.z = z;
	Update();
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
	Update();
}

void Object::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x, y, z);
	Update();
}

void Object::SetRotationX(float x)
{
	m_rotation.x = x;
	Update();
}

void Object::SetRotationY(float y)
{
	m_rotation.y = y;
	Update();
}

void Object::SetRotationZ(float z)
{
	m_rotation.z = z;
	Update();
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
	Update();
}

void Object::SetScale(float x, float y, float z)
{
	m_scale = XMFLOAT3(x, y, z);
	Update();
}

void Object::SetScaleX(float x)
{
	m_scale.x = x;
	Update();
}

void Object::SetScaleY(float y)
{
	m_scale.y = y;
	Update();
}

void Object::SetScaleZ(float z)
{
	m_scale.z = z;
	Update();
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

void Object::SetTexTransformPos(float px, float py, float pz)
{
	m_texPos = XMFLOAT3(px, py, pz);
	Update();
}

void Object::SetTexTransformRotate(float rx, float ry, float rz)
{
	m_texRot = XMFLOAT3(rx, ry, rz);
	Update();
}

void Object::SetTexTransformScale(float sx, float sy, float sz)
{
	m_texSca = XMFLOAT3(sx, sy, sz);
	Update();
}

XMMATRIX Object::GetTexTransform()
{
	return m_texTrans;
}

void Object::SetTextureID(UINT textureID)
{
	m_textureID = textureID;
}

UINT Object::GetTextureID()
{
	return m_textureID;
}

void Object::SetMaterialAmbient(XMFLOAT4 values)
{
	m_material.Ambient = values;
}

void Object::SetMaterialAmbient(float r, float g, float b, float a)
{
	m_material.Ambient = XMFLOAT4(r, g, b, a);
}

void Object::SetMaterialDiffuse(XMFLOAT4 values)
{
	m_material.Diffuse = values;
}

void Object::SetMaterialDiffuse(float r, float g, float b, float a)
{
	m_material.Diffuse = XMFLOAT4(r, g, b, a);
}

void Object::SetMaterialSpecular(XMFLOAT4 values)
{
	m_material.Specular = values;
}

void Object::SetMaterialSpecular(float r, float g, float b, float specPower)
{
	m_material.Specular = XMFLOAT4(r, g, b, specPower);
}

void Object::SetMaterialProperties(XMFLOAT4 values)
{
	m_material.Properties = values;
}

void Object::SetMaterialProperties(float reflectiveness, float metallic, float roughness, float a)
{
	m_material.Properties = XMFLOAT4(reflectiveness, metallic, roughness, a);
}

void Object::SetMaterial(Material material)
{
	m_material = material;
}

Material Object::GetMaterial()
{
	return m_material;
}

void Object::SetTag(unsigned int tag)
{
	m_tag = tag;
}

unsigned int Object::GetTag()
{
	return m_tag;
}

void Object::SetName(std::string name)
{
	m_name = name;
}

std::string Object::GetName()
{
	return m_name;
}

void Object::SetDisabled(bool disabled)
{
	m_disabled = disabled;
}

bool Object::GetDisabled()
{
	return m_disabled;
}

void Object::SetDynamic(bool dynamic)
{
	m_dynamic = dynamic;
}

bool Object::GetDynamic()
{
	return m_dynamic;
}

void Object::SetTexture(ID3D11ShaderResourceView* texture, UINT slot)
{
	m_texture[slot] = texture;
}

ID3D11ShaderResourceView* Object::GetTexture(UINT slot)
{
	return m_texture[slot];
}

void Object::Update()
{
	//object world Update
	XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	XMMATRIX rotation = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(m_rotation.y)), 
		XMMatrixRotationX(XMConvertToRadians(m_rotation.x))), XMMatrixRotationZ(XMConvertToRadians(m_rotation.z)));
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	m_world = XMMatrixMultiply(XMMatrixMultiply(scaling, rotation), translation);

	//tex transforma Update
	scaling = XMMatrixScaling(m_texSca.x, m_texSca.y, m_texSca.z);
	rotation = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(m_texRot.y)),
		XMMatrixRotationX(XMConvertToRadians(m_texRot.x))), XMMatrixRotationZ(XMConvertToRadians(m_texRot.z)));
	translation = XMMatrixTranslation(m_texPos.x, m_texPos.y, m_texPos.z);
	m_texTrans = XMMatrixMultiply(XMMatrixMultiply(scaling, rotation), translation);
}

void Object::Render()
{

}