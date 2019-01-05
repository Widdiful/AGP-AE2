#include <d3d11.h>
#include <d3d11.h>
#include <math.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

#pragma once
class Camera
{
private:
	float m_x, m_y, m_z;
	float m_dx, m_dy, m_dz;
	float m_camera_rotation, m_camera_pitch;

	XMVECTOR m_position, m_lookat, m_up;
public:
	Camera(float x, float y, float z, float rotation);
	~Camera();
	void Rotate(float val);
	void Rotate(float val, float cap);
	void Pitch(float val);
	void Pitch(float val, float cap);
	void Pitch(float val, float min, float max);
	void Forward(float distance);
	void Strafe(float distance);
	void Up(float distance);
	void LookAt(float x, float y, float z);
	void LerpAt(float x, float y, float z, float lerpSpeed);
	XMMATRIX GetViewMatrix();

	float GetX();
	float GetY();
	float GetZ();

	float GetDX();
	float GetDZ();

	float GetYRot();

	void SetX(float val);
	void SetY(float val);
	void SetZ(float val);

	void MoveTowardsX(float val, float lerpSpeed);
	void MoveTowardsY(float val, float lerpSpeed);
	void MoveTowardsZ(float val, float lerpSpeed);
};

