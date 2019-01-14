#include "CameraControl.h"
#include "Level.h"



CameraControl::CameraControl(Camera * camera, SceneNode * target, SceneNode* cameraNode, InputManager* input)
{
	// Initialise default variables
	m_name = "Camera Control";

	m_camera = camera;
	m_target = target;
	m_cameraNode = cameraNode;
	m_input = input;

	m_moveSpeed = 0.005;
	m_rotateSpeed = 0.5;
	m_gripRotateSpeed = 0.1f;

	m_offsetX = 0;
	m_offsetY = 25;
	m_offsetZ = -50;

	m_FPmoveSpeed = 0.05;
	m_FProtateSpeed = 0.5;
	m_FPgripRotateSpeed = 0.1f;

	m_FPoffsetX = 0;
	m_FPoffsetY = 0;
	m_FPoffsetZ = -2;

	m_offsets.push_back(Vector3(m_offsetX, m_offsetY, m_offsetZ));
	m_offsets.push_back(Vector3(m_FPoffsetX, m_FPoffsetY, m_FPoffsetZ));
	m_moveSpeeds.push_back(m_moveSpeed);
	m_moveSpeeds.push_back(m_FPmoveSpeed);
	m_rotateSpeeds.push_back(m_rotateSpeed);
	m_rotateSpeeds.push_back(m_FProtateSpeed);
	m_gripSpeeds.push_back(m_gripRotateSpeed);
	m_gripSpeeds.push_back(m_FPgripRotateSpeed);

	m_zoomSpeed = 0.1;
	m_mouseSensitivity = 0.5f;
}

CameraControl::~CameraControl()
{
}

void CameraControl::Update()
{
	Component::Update();

	// Toggle first-person view
	if (m_input->IsKeyBeganPressed(DIK_Q)) {
		m_cameraMode = (m_cameraMode + 1) % m_offsets.size();
		m_target->SetVisible(!m_target->GetVisible());
	}

	// Rotate camera from keyboard input
	if (m_input->IsKeyPressed(DIK_LEFT)) {
		m_node->AddYRot(m_gripSpeeds[m_cameraMode] * Time::getInstance().deltaTime);
	}
	if (m_input->IsKeyPressed(DIK_RIGHT)) {
		m_node->AddYRot(-m_gripSpeeds[m_cameraMode] * Time::getInstance().deltaTime);
	}
	if (m_input->IsKeyPressed(DIK_UP)) {
		m_node->AddXRot(m_gripSpeeds[m_cameraMode] * Time::getInstance().deltaTime);
	}
	if (m_input->IsKeyPressed(DIK_DOWN)) {
		m_node->AddXRot(-m_gripSpeeds[m_cameraMode] * Time::getInstance().deltaTime);
	}

	// Rotate camera from input
	LONG xMove = m_input->GetMouseState().lX * m_mouseSensitivity;
	LONG yMove = m_input->GetMouseState().lY * m_mouseSensitivity;
	if (xMove != 0) {
		m_node->AddYRot(m_gripSpeeds[m_cameraMode] * Time::getInstance().deltaTime * xMove);
	}
	if (yMove != 0) {
		m_node->AddXRot(m_gripSpeeds[m_cameraMode] * Time::getInstance().deltaTime * yMove);
	}

	// Zoom camera on scroll
	if (m_input->GetMouseState().lZ != 0) {
		m_offsets[0].z += m_input->GetMouseState().lZ * m_zoomSpeed;
	}

	// Move camera appropriately
	m_node->SetPos(m_target->GetXPos(), m_target->GetYPos(), m_target->GetZPos());
	m_cameraNode->SetPos(m_offsets[m_cameraMode].x, m_offsets[m_cameraMode].y, m_offsets[m_cameraMode].z);
	m_camera->MoveTowardsX(m_cameraNode->GetWorldXPos(), m_moveSpeeds[m_cameraMode] * Time::getInstance().deltaTime);
	m_camera->MoveTowardsY(m_cameraNode->GetWorldYPos(), m_moveSpeeds[m_cameraMode] * Time::getInstance().deltaTime);
	m_camera->MoveTowardsZ(m_cameraNode->GetWorldZPos(), m_moveSpeeds[m_cameraMode] * Time::getInstance().deltaTime);
	m_camera->LerpAt(m_target->GetXPos(), m_target->GetYPos(), m_target->GetZPos(), m_rotateSpeeds[m_cameraMode]);
}
