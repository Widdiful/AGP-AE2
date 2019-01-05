#pragma once
#include "Component.h"
#include "Camera.h"
#include "SceneNode.h"
class CameraControl :
	public Component
{
private:
	Camera* m_camera;
	SceneNode* m_target;
	SceneNode* m_cameraNode;
	InputManager* m_input;
	float m_moveSpeed, m_rotateSpeed, m_gripRotateSpeed;
	float m_offsetX, m_offsetY, m_offsetZ;

public:
	CameraControl(Camera* camera, SceneNode* target, SceneNode* cameraNode, InputManager* input);
	~CameraControl();

	void Update();
};

