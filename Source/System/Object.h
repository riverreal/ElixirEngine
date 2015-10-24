#pragma once

#include <DirectXMath.h>
#include "../Helper/TypeHelper.h"

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

	void Update();
	void Render();

private:
	///<summary>
	///Rotation is stored in Degrees
	///</summary>
	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_scale;

	offsetData m_offset;
	DirectX::XMMATRIX m_world;
};