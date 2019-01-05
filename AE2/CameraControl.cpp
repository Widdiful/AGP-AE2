#include "CameraControl.h"



CameraControl::CameraControl(Camera * camera, scene_node * target, scene_node* cameraNode, InputManager* input)
{
	m_name = "Camera Control";

	m_camera = camera;
	m_target = target;
	m_cameraNode = cameraNode;
	m_input = input;

	m_moveSpeed = 0.0005;
	m_rotateSpeed = 0.005;
	m_gripRotateSpeed = 0.02f;

	m_offsetX = 0;
	m_offsetY = 25;
	m_offsetZ = -50;
}

CameraControl::~CameraControl()
{
}

void CameraControl::Update()
{
	Component::Update();
	if (m_input->IsKeyPressed(DIK_LEFT)) {
		m_node->AddYRot(m_gripRotateSpeed);
	}
	if (m_input->IsKeyPressed(DIK_RIGHT)) {
		m_node->AddYRot(-m_gripRotateSpeed);
	}
	if (m_input->IsKeyPressed(DIK_UP)) {
		m_node->AddXRot(m_gripRotateSpeed);
	}
	if (m_input->IsKeyPressed(DIK_DOWN)) {
		m_node->AddXRot(-m_gripRotateSpeed);
	}

	m_node->SetPos(m_target->GetXPos(), m_target->GetYPos(), m_target->GetZPos());
	m_cameraNode->SetPos(m_offsetX, m_offsetY, m_offsetZ);
	m_camera->MoveTowardsX(m_cameraNode->GetWorldXPos(), m_moveSpeed);
	m_camera->MoveTowardsY(m_cameraNode->GetWorldYPos(), m_moveSpeed);
	m_camera->MoveTowardsZ(m_cameraNode->GetWorldZPos(), m_moveSpeed);
	m_camera->LerpAt(m_target->GetXPos(), m_target->GetYPos(), m_target->GetZPos(), m_rotateSpeed);
}
