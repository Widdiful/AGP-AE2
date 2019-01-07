#pragma once
#include "Camera.h"
#include "SceneNode.h"
#include "InputManager.h"
#include "Skybox.h"
#include "Timer.h"

class Level
{
public:
	SceneNode* m_rootNode;
	SceneNode* m_cameraGripNode;
	SceneNode* m_cameraNode;
	Camera* m_camera;
	InputManager* m_input;
	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;

	ID3D11RasterizerState*		m_pRasterSolid = 0;
	ID3D11RasterizerState*		m_pRasterSkybox = 0;
	ID3D11DepthStencilState*	m_pDepthWriteSolid = 0;
	ID3D11DepthStencilState*	m_pDepthWriteSkybox = 0;

	XMMATRIX m_world, m_projection, m_view;
	Skybox* m_skybox;
	Timer* m_timer;

public:
	Level(InputManager* input, ID3D11Device* device, ID3D11DeviceContext* context, Skybox* skybox);
	~Level();
	virtual void InitialiseLevel();
	virtual void StartComponents();
	virtual void Update();
	virtual void Restart();
	virtual void CleanUp();

	double GetDeltaTime();
};

