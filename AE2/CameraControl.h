#pragma once
#include "Component.h"
#include "Camera.h"
#include "scene_node.h"
class CameraControl :
	public Component
{
private:
	Camera* m_camera;
	scene_node* m_target;
	scene_node* m_cameraNode;
	InputManager* m_input;
	float m_moveSpeed, m_rotateSpeed, m_gripRotateSpeed;
	float m_offsetX, m_offsetY, m_offsetZ;

public:
	CameraControl(Camera* camera, scene_node* target, scene_node* cameraNode, InputManager* input);
	~CameraControl();

	void Update();
};

