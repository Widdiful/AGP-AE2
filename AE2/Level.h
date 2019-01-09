#pragma once
#include "Camera.h"
#include "SceneNode.h"
#include "InputManager.h"
#include "Skybox.h"
#include "Time.h"
#include "text2D.h"
#include "UIManager.h"

class Level
{
protected:
	SceneNode* m_rootNode;
	SceneNode* m_cameraGripNode;
	SceneNode* m_cameraNode;
	Camera* m_camera;
	InputManager* m_input;
	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	vector<SceneNode*> m_nodes;
	Text2D* m_2DText;
	Component* m_uiManager;

	ID3D11RasterizerState*		m_pRasterSolid = 0;
	ID3D11RasterizerState*		m_pRasterSkybox = 0;
	ID3D11DepthStencilState*	m_pDepthWriteSolid = 0;
	ID3D11DepthStencilState*	m_pDepthWriteSkybox = 0;

	XMMATRIX m_world, m_projection, m_view;
	Skybox* m_skybox;

	vector<string> levelText;

	int m_coinCount;
	int m_redCoinCount;
	bool m_complete;

public:
	Level(string file, InputManager* input, ID3D11Device* device, ID3D11DeviceContext* context, Skybox* skybox);
	~Level();
	virtual void InitialiseLevel();
	virtual void StartComponents();
	virtual void Update();
	virtual void Restart();
	virtual void CleanUp();
	virtual void CompleteLevel();

	int GetCoinCount();
	int GetRedCoinCount();
	bool IsComplete();
};

