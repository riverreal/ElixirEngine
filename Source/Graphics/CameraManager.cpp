#include "CameraManager.h"

using namespace DirectX;

Camera::Camera()
	:m_positionX(0),
	m_positionY(0),
	m_positionZ(0),
	m_rotationX(0),
	m_rotationY(0),
	m_rotationZ(0)
{
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::Render()
{
	XMFLOAT3 up, pos, lookAt;
	XMVECTOR upVec, posVec, lookAtVec;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVec = XMLoadFloat3(&up);

	pos.x = m_positionX;
	pos.y = m_positionY;
	pos.z = m_positionZ;

	posVec = XMLoadFloat3(&pos);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVec = XMLoadFloat3(&lookAt);

	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVec = XMVector3TransformCoord(lookAtVec, rotationMatrix);
	upVec = XMVector3TransformCoord(upVec, rotationMatrix);

	lookAtVec = XMVectorAdd(posVec, lookAtVec);
	m_viewMatrix = XMMatrixLookAtLH(posVec, lookAtVec, upVec);
}

void Camera::GetViewMatrix(XMMATRIX& destination)
{
	destination = m_viewMatrix;
}