#include "CameraManager.h"

using namespace DirectX;

Camera::Camera()
	:m_position(0.0f, 0.0f, 0.0f),
	m_right(1.0f, 0.0f, 0.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_look(0.0f, 0.0f, 1.0f)
{
}

Camera::~Camera()
{
}

void Camera::SetLookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_position, pos);
	XMStoreFloat3(&m_look, L);
	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);
}

void Camera::SetLookAt(const XMFLOAT3& pos, const XMFLOAT3 &target, const XMFLOAT3 &up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	SetLookAt(P, T, U);
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(DirectX::XMFLOAT3 pos)
{
	m_position = pos;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

void Camera::SetProjection(const DirectX::XMMATRIX & proj)
{
	XMStoreFloat4x4(&m_projectionMatrix, proj);
}

void Camera::SetUp(DirectX::XMFLOAT3 up)
{
	m_up = up;
}

void Camera::SetRight(DirectX::XMFLOAT3 right)
{
	m_right = right;
}

void Camera::SetLook(DirectX::XMFLOAT3 look)
{
	m_look = look;
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&m_position);
}

XMFLOAT3 Camera::GetPosition() const
{
	return m_position;
}

XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&m_right);
}

XMFLOAT3 Camera::GetRight() const
{
	return m_right;
}

XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&m_look);
}

XMFLOAT3 Camera::GetLook() const
{
	return m_look;
}

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&m_up);
}

XMFLOAT3 Camera::GetUp() const
{
	return m_up;
}

XMMATRIX Camera::GetViewMatrix() const
{
	return XMLoadFloat4x4(&m_viewMatrix);
}
DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
	return XMLoadFloat4x4(&m_projectionMatrix);
}

void Camera::Strafe(float distance)
{
	XMVECTOR s = XMVectorReplicate(distance);
	XMVECTOR r = XMLoadFloat3(&m_right);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float distance)
{
	XMVECTOR s = XMVectorReplicate(distance);
	XMVECTOR l = XMLoadFloat3(&m_look);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
}

void Camera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
}

void Camera::Update()
{
	XMVECTOR R = XMLoadFloat3(&m_right);
	XMVECTOR U = XMLoadFloat3(&m_up);
	XMVECTOR L = XMLoadFloat3(&m_look);
	XMVECTOR P = XMLoadFloat3(&m_position);

	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	R = XMVector3Cross(U, L);

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_look, L);
	XMStoreFloat3(&m_up, U);

	m_viewMatrix(0, 0) = m_right.x;
	m_viewMatrix(1, 0) = m_right.y;
	m_viewMatrix(2, 0) = m_right.z;
	m_viewMatrix(3, 0) = x;

	m_viewMatrix(0, 1) = m_up.x;
	m_viewMatrix(1, 1) = m_up.y;
	m_viewMatrix(2, 1) = m_up.z;
	m_viewMatrix(3, 1) = y;

	m_viewMatrix(0, 2) = m_look.x;
	m_viewMatrix(1, 2) = m_look.y;
	m_viewMatrix(2, 2) = m_look.z;
	m_viewMatrix(3, 2) = z;

	m_viewMatrix(0, 3) = 0.0f;
	m_viewMatrix(1, 3) = 0.0f;
	m_viewMatrix(2, 3) = 0.0f;
	m_viewMatrix(3, 3) = 1.0f;
	
	/*
	XMFLOAT3 up, pos, lookAt;
	XMVECTOR upVec, posVec, lookAtVec;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVec = XMLoadFloat3(&up);

	posVec = XMLoadFloat3(&m_position);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVec = XMLoadFloat3(&lookAt);

	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll = m_rotation.z * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVec = XMVector3TransformCoord(lookAtVec, rotationMatrix);
	upVec = XMVector3TransformCoord(upVec, rotationMatrix);

	lookAtVec = XMVectorAdd(posVec, lookAtVec);
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(posVec, lookAtVec, upVec));
	*/
}

void Camera::calcOnce()
{
	XMFLOAT3 up, pos, lookAt;
	XMVECTOR upVec, posVec, lookAtVec;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVec = XMLoadFloat3(&up);

	posVec = XMLoadFloat3(&m_position);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVec = XMLoadFloat3(&lookAt);

	pitch = 0 * 0.0174532925f;
	yaw = 0 * 0.0174532925f;
	roll = 0 * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVec = XMVector3TransformCoord(lookAtVec, rotationMatrix);
	upVec = XMVector3TransformCoord(upVec, rotationMatrix);

	lookAtVec = XMVectorAdd(posVec, lookAtVec);
	XMMATRIX result = XMMatrixLookAtLH(posVec, lookAtVec, upVec);
	 XMStoreFloat4x4(&m_viewMatrix, result);
}

