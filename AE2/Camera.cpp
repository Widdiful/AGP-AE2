#include "Camera.h"


Camera::Camera(float x, float y, float z, float rotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotation = rotation;
	m_camera_pitch = 0;

	m_dx = sin(rotation * (XM_PI / 180.0f));
	m_dy = sin(m_camera_pitch * (XM_PI / 180.0f));
	m_dz = cos(rotation * (XM_PI / 180.0f));
}

Camera::~Camera()
{
}

void Camera::Rotate(float val)
{
	m_camera_rotation += val;
	m_dx = sin(m_camera_rotation * (XM_PI / 180.0f));
	m_dz = cos(m_camera_rotation * (XM_PI / 180.0f));
}

void Camera::Rotate(float val, float cap)
{
	m_camera_rotation += val;
	if (m_camera_rotation < -cap) m_camera_rotation = -cap;
	if (m_camera_rotation > cap) m_camera_rotation = cap;

	m_dx = sin(m_camera_rotation * (XM_PI / 180.0f));
	m_dz = cos(m_camera_rotation * (XM_PI / 180.0f));
}

void Camera::Pitch(float val)
{
	m_camera_pitch += val;
	m_dy = sin(m_camera_pitch * (XM_PI / 180.0f));
}

void Camera::Pitch(float val, float cap)
{
	m_camera_pitch += val;
	if (m_camera_pitch < -cap) m_camera_pitch = -cap;
	if (m_camera_pitch > cap) m_camera_pitch = cap;
	m_dy = sin(m_camera_pitch * (XM_PI / 180.0f));
}

void Camera::Pitch(float val, float min, float max)
{
	m_camera_pitch += val;
	if (m_camera_pitch < min) m_camera_pitch = min;
	if (m_camera_pitch > max) m_camera_pitch = max;
	m_dy = sin(m_camera_pitch * (XM_PI / 180.0f));
}

void Camera::Forward(float distance)
{
	m_x += m_dx * distance;
	m_y += m_dy * distance;
	m_z += m_dz * distance;
}

void Camera::Strafe(float distance)
{
	XMVECTOR direction;

	direction = XMVector3Cross(m_position - m_lookat, m_up);

	m_x += XMVectorGetX(direction) * distance;
	m_z += XMVectorGetZ(direction) * distance;
}

void Camera::Up(float distance)
{
	m_y += distance;
}

void Camera::LookAt(float x, float y, float z)
{
	m_dx = x - m_x;
	m_dy = y - m_y;
	m_dz = z - m_z;
}

void Camera::LerpAt(float x, float y, float z, float lerpSpeed)
{
	float dx = x - m_x;
	float dy = y - m_y;
	float dz = z - m_z;

	m_dx = (m_dx + (lerpSpeed * (dx - m_dx)));
	m_dy = (m_dy + (lerpSpeed * (dy - m_dy)));
	m_dz = (m_dz + (lerpSpeed * (dz - m_dz)));
}

XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, 0.0);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	return XMMatrixLookAtLH(m_position, m_lookat, m_up);
}

float Camera::GetX()
{
	return m_x;
}

float Camera::GetY()
{
	return m_y;
}

float Camera::GetZ()
{
	return m_z;
}

float Camera::GetDX()
{
	return m_dx;
}

float Camera::GetDZ()
{
	return m_dz;
}

float Camera::GetYRot()
{
	return m_camera_rotation;
}

void Camera::SetX(float val)
{
	m_x = val;
}

void Camera::SetY(float val)
{
	m_y = val;
}

void Camera::SetZ(float val)
{
	m_z = val;
}

void Camera::MoveTowardsX(float val, float lerpSpeed)
{
	m_x = (m_x + (lerpSpeed * (val - m_x)));
}

void Camera::MoveTowardsY(float val, float lerpSpeed)
{
	m_y = (m_y + (lerpSpeed * (val - m_y)));
}

void Camera::MoveTowardsZ(float val, float lerpSpeed)
{
	m_z = (m_z + (lerpSpeed * (val - m_z)));
}
