#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "../Helper/TypeHelper.h"
#include "../Helper/LightHelper.h"

class Object
{
public:
	Object();
	Object(bool staticObject);
	~Object();

	void SetPosition(DirectX::XMFLOAT3 position);
	void SetPosition(float x, float y, float z);
	void SetPostiionX(float x);
	void SetPositionY(float y);
	void SetPositionZ(float z);
	DirectX::XMFLOAT3 GetPosition();
	float GetPositionX();
	float GetPositionY();
	float GetPositionZ();

	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	void SetRotation(DirectX::XMFLOAT3 rotation);
	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	void SetRotation(float x, float y, float z);
	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	void SetRotationX(float x);
	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	void SetRotationY(float y);
	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	void SetRotationZ(float z);
	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	DirectX::XMFLOAT3 GetRotation();
	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	float GetRotationX();
	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	float GetRotationY();
	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	float GetRotationZ();

	void SetScale(DirectX::XMFLOAT3 scale);
	void SetScale(float x, float y, float z);
	void SetScaleX(float x);
	void SetScaleY(float y);
	void SetScaleZ(float z);
	DirectX::XMFLOAT3 GetScale();
	float GetScaleX();
	float GetScaleY();
	float GetScaleZ();

	void SetOffset(offsetData offset);
	offsetData GetOffset();

	DirectX::XMMATRIX GetWorldMatrix();

	void SetTexTransformPos(float px, float py, float pz);
	void SetTexTransformScale(float sx, float sy, float sz);
	void SetTexTransformRotate(float rx, float ry, float rz);
	DirectX::XMMATRIX GetTexTransform();

	void SetTextureID(UINT textureID);
	UINT GetTextureID();

	void SetMaterialAmbient(DirectX::XMFLOAT4 values);
	void SetMaterialAmbient(float r, float g, float b, float a);
	void SetMaterialDiffuse(DirectX::XMFLOAT4 values);
	void SetMaterialDiffuse(float r, float g, float b, float a);
	//SpecularColor.rgb, SpecularPower
	void SetMaterialSpecular(DirectX::XMFLOAT4 values);
	void SetMaterialSpecular(float r, float g, float b, float specPower);
	//Reflectiveness, Metallic, Roughness, unused
	void SetMaterialProperties(DirectX::XMFLOAT4 values);
	void SetMaterialProperties(float reflectiveness, float metallic, float roughness, float a);

	void SetMaterial(Material material);
	Material GetMaterial();

	void SetTexture(ID3D11ShaderResourceView* texture, UINT slot);
	ID3D11ShaderResourceView* GetTexture(UINT slot);

	void Update();
	void Render();

private:
	///<summary>
	///Stores World Matrix Rotation data in a separate variable. Note: 
	///Rotation is stored in Degrees
	///</summary>
	DirectX::XMFLOAT3 m_rotation;
	//Stores World Matrix position data in a separate variable
	DirectX::XMFLOAT3 m_position;
	//Stores World Matrix scale data in a separate variable
	DirectX::XMFLOAT3 m_scale;

	///<summary>
	///Stores Texture Transformation Matrix rotation data in a separate variable.
	///Note: Rotation is stored in Degrees.
	///</summary>
	DirectX::XMFLOAT3 m_texRot;
	//Stores Texture Transformation Matrix position data in a separete variable.
	DirectX::XMFLOAT3 m_texPos;
	//Stores Texture Transformation Matrix scale data in a separate variable.
	DirectX::XMFLOAT3 m_texSca;

	//Stores index count, index offset, and vertex offset of the object for rendering.
	offsetData m_offset;
	DirectX::XMMATRIX m_world;
	DirectX::XMMATRIX m_texTrans;
	UINT m_textureID;
	//Not implemented material manager
	UINT m_materialID;
	//Temporal material storage will be removed once material manager is completed
	Material m_material;
	ID3D11ShaderResourceView* m_texture[3];
};