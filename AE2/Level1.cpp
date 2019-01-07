#include "Level1.h"
#include "InputManager.h"
#include "SceneNode.h"
#include "Component.h"
#include "Actor.h"
#include "Player.h"
#include "CameraControl.h"
#include "Enemy.h"
#include "UIManager.h"
#include "Pickup.h"



Level1::Level1(InputManager* input, ID3D11Device* device, ID3D11DeviceContext* context, Skybox* skybox) : Level(input, device, context, skybox)
{
	// Load base level assets
	// Always have this in level constructors!!
	InitialiseLevel();
}


Level1::~Level1()
{
}

void Level1::InitialiseLevel()
{
	// Set up graphics
	m_skybox->SetTexture("assets/skybox01.dds");

	m_2DText = new Text2D("assets/font1.bmp", m_pD3DDevice, m_pImmediateContext);

	m_model = new Model(m_pD3DDevice, m_pImmediateContext);
	m_model->LoadObjModel((char*)"assets/Sphere.obj");
	m_model->AddTexture((char*)"assets/gaogaigar.bmp");

	m_model1 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_model1->LoadObjModel((char*)"assets/Sphere.obj");
	m_model1->AddTexture((char*)"assets/gaogaigar.bmp");

	m_particleGenerator = new ParticleGenerator(m_pD3DDevice, m_pImmediateContext);
	m_particleGenerator->CreateParticle();

	// Set up scene nodes
	m_playerNode = new SceneNode("Player");
	m_enemyNode = new SceneNode("Enemy");

	m_playerNode->SetModel(m_model);
	m_enemyNode->SetModel(m_model1);
	m_enemyNode->SetXPos(-10);
	m_enemyNode->SetYPos(10);

	m_rootNode->addChildNode(m_playerNode);
	m_rootNode->addChildNode(m_enemyNode);

	int coinCount = 10;
	int redCoinCount = 8;
	for (int i = 0; i < coinCount; i++) {
		m_coinModels.push_back(new Model(m_pD3DDevice, m_pImmediateContext));
		m_coinModels.back()->LoadObjModel((char*)"assets/Sphere.obj");
		m_coinModels.back()->AddTexture((char*)"assets/gaogaigar.bmp");
		m_coins.push_back(new SceneNode("Coin"));
		m_coins.back()->SetModel(m_coinModels.back());
		m_coins.back()->SetXPos((i % 2) * 10);
		m_coins.back()->SetZPos(i * 5);
		m_coins.back()->AddComponent(new Pickup());
		m_rootNode->addChildNode(m_coins.back());
	}
	for (int i = 0; i < redCoinCount; i++) {
		m_redCoinModels.push_back(new Model(m_pD3DDevice, m_pImmediateContext));
		m_redCoinModels.back()->LoadObjModel((char*)"assets/Sphere.obj");
		m_redCoinModels.back()->AddTexture((char*)"assets/gaogaigar.bmp");
		m_redCoins.push_back(new SceneNode("Red Coin"));
		m_redCoins.back()->SetModel(m_redCoinModels.back());
		m_redCoins.back()->SetXPos(-(i % 2) * 10);
		m_redCoins.back()->SetZPos(-i * 5);
		m_redCoins.back()->AddComponent(new Pickup());
		m_rootNode->addChildNode(m_redCoins.back());
	}

	
	// Set up components
	m_playerNode->AddComponent(new Player(true, m_input, m_cameraGripNode));
	m_playerNode->AddComponent(new UIManager(m_2DText));
	m_enemyNode->AddComponent(new Enemy(true));
	m_cameraGripNode->AddComponent(new CameraControl(m_camera, m_playerNode, m_cameraNode, m_input));

	StartComponents();
}

void Level1::Update()
{
	Level::Update();
	m_particleGenerator->Draw(&m_view, &m_projection, new XMFLOAT3(m_camera->GetX(), m_camera->GetY(), m_camera->GetZ()));
}

void Level1::Restart()
{
	CleanUp();
	InitialiseLevel();
	StartComponents();
}

void Level1::CleanUp()
{
	delete m_2DText;
	m_2DText = nullptr;
	delete m_model;
	m_model = nullptr;
	delete m_model1;
	m_model1 = nullptr;
	delete m_particleGenerator;
	m_particleGenerator = nullptr;
	delete m_playerNode;
	m_playerNode = nullptr;
	delete m_enemyNode;
	m_enemyNode = nullptr;
}
