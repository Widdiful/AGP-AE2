#pragma once
#include "Component.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Vector3.h"
class CameraControl :
	public Component
{
private:
	Camera* m_camera;
	SceneNode* m_target;
	SceneNode* m_cameraNode;
	InputManager* m_input;

	int m_cameraMode = 0;

	// Third person camera variables
	float m_moveSpeed, m_rotateSpeed, m_gripRotateSpeed;
	float m_offsetX, m_offsetY, m_offsetZ;

	// First person camera variables
	float m_FPmoveSpeed, m_FProtateSpeed, m_FPgripRotateSpeed;
	float m_FPoffsetX, m_FPoffsetY, m_FPoffsetZ;

	vector<Vector3> m_offsets;
	vector<float> m_moveSpeeds;
	vector<float> m_rotateSpeeds;
	vector<float> m_gripSpeeds;

public:
	CameraControl(Camera* camera, SceneNode* target, SceneNode* cameraNode, InputManager* input);
	~CameraControl();

	void Update();
};

