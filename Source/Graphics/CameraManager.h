#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	~Camera();

	//Setters

	void SetLookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void SetLookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up);
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 pos);
	void SetRotation(float x, float y, float z);
	//void SetProjection(const DirectX::XMMATRIX& proj); //temporary
	void SetUp(DirectX::XMFLOAT3 up);
	void SetRight(DirectX::XMFLOAT3 right);
	void SetLook(DirectX::XMFLOAT3 look);

	//Getters

	DirectX::XMVECTOR GetRightXM() const;
	DirectX::XMFLOAT3 GetRight() const;

	DirectX::XMVECTOR GetUpXM() const;
	DirectX::XMFLOAT3 GetUp() const;

	DirectX::XMVECTOR GetLookXM() const;
	DirectX::XMFLOAT3 GetLook() const;

	DirectX::XMVECTOR GetPositionXM() const;
	DirectX::XMFLOAT3 GetPosition() const;
	
	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;

	//Camera Movement
	void Walk(float distance);
	void Strafe(float distance);

	void Pitch(float angle);
	void RotateY(float angle);

	void Update();

	void calcOnce();

	void BuildProjection();
	void SetScreenAspect(float screenAspect);
	//Set Field of View
	void SetFOV(float fov);
	float GetFOV();

private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_right;
	DirectX::XMFLOAT3 m_up;
	DirectX::XMFLOAT3 m_look;
	DirectX::XMFLOAT3 m_rotation;

	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;

	//field of view
	float m_FoV;
	float m_screenAspect;
};