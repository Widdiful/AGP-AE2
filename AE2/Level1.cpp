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
#include "LevelCube.h"



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
	Level::InitialiseLevel();
	// Set up graphics
	m_skybox->SetTexture("assets/skybox01.dds");

	m_2DText = new Text2D("assets/font1.bmp", m_pD3DDevice, m_pImmediateContext);

	m_playerModel = new Model(m_pD3DDevice, m_pImmediateContext);
	m_playerModel->LoadObjModel((char*)"assets/Sphere.obj");
	m_playerModel->AddTexture((char*)"assets/gaogaigar.bmp");

	m_enemyModel = new Model(m_pD3DDevice, m_pImmediateContext);
	m_enemyModel->LoadObjModel((char*)"assets/Sphere.obj");
	m_enemyModel->AddTexture((char*)"assets/gaogaigar.bmp");

	m_Box1Model = new Model(m_pD3DDevice, m_pImmediateContext);
	m_Box1Model->LoadObjModel((char*)"assets/Cube.obj");
	m_Box1Model->AddTexture((char*)"assets/gaogaigar.bmp");

	m_particleGenerator = new ParticleGenerator(m_pD3DDevice, m_pImmediateContext);
	m_particleGenerator->CreateParticle();

	// Set up scene nodes
	m_playerNode = new SceneNode("Player");
	m_enemyNode = new SceneNode("Enemy");
	m_Box1 = new SceneNode("Box1");

	m_playerNode->SetModel(m_playerModel);
	m_enemyNode->SetModel(m_enemyModel);
	m_Box1->SetModel(m_Box1Model);
	m_enemyNode->SetXPos(-100);
	m_enemyNode->SetYPos(10);
	m_Box1->SetScale(100, 5, 200);
	m_Box1->SetYPos(-10);

	m_rootNode->addChildNode(m_playerNode);
	m_rootNode->addChildNode(m_enemyNode);
	m_rootNode->addChildNode(m_Box1);

	m_coinCount = 10;
	m_redCoinCount = 8;
	for (int i = 0; i < m_coinCount; i++) {
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
	for (int i = 0; i < m_redCoinCount; i++) {
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
	m_Box1->AddComponent(new LevelCube());

	StartComponents();
	m_uiManager = m_rootNode->GetComponentInChildren("UI Manager");
}

void Level1::Update()
{
	Level::Update();
	m_particleGenerator->Draw(&m_view, &m_projection, new XMFLOAT3(m_camera->GetX(), m_camera->GetY(), m_camera->GetZ()));
	m_uiManager->Update();
}

void Level1::Restart()
{
	CleanUp();
	InitialiseLevel();
}

void Level1::CleanUp()
{
	Level::CleanUp();
	delete m_2DText;
	m_2DText = nullptr;
	delete m_playerModel;
	m_playerModel = nullptr;
	delete m_enemyModel;
	m_enemyModel = nullptr;
	delete m_particleGenerator;
	m_particleGenerator = nullptr;
	delete m_playerNode;
	m_playerNode = nullptr;
	delete m_enemyNode;
	m_enemyNode = nullptr;
	
	for (int i = 0; i < m_coins.size(); i++) {
		delete m_coinModels[i];
		m_coinModels[i] = nullptr;
		delete m_coins[i];
		m_coins[i] = nullptr;
	}
	m_coins.clear();
	m_coinModels.clear();

	for (int i = 0; i < m_redCoins.size(); i++) {
		delete m_redCoinModels[i];
		m_redCoinModels[i] = nullptr;
		delete m_redCoins[i];
		m_redCoins[i] = nullptr;
	}
	m_redCoins.clear();
	m_redCoinModels.clear();
}
