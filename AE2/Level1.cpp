#include "Level1.h"
#include "InputManager.h"
#include "SceneNode.h"
#include "Component.h"
#include "Actor.h"
#include "Player.h"
#include "CameraControl.h"
#include "Enemy.h"
#include "UIManager.h"



Level1::Level1(InputManager* input, ID3D11Device* device, ID3D11DeviceContext* context, Skybox* skybox) : Level(input, device, context, skybox)
{
}


Level1::~Level1()
{
}

void Level1::InitialiseLevel()
{
	Level::InitialiseLevel();

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

	m_playerNode = new SceneNode("Player");
	m_node2 = new SceneNode("Enemy");

	m_playerNode->SetModel(m_model);
	m_node2->SetModel(m_model1);
	m_node2->SetXPos(-10);
	m_node2->SetYPos(10);

	m_rootNode->addChildNode(m_playerNode);
	m_rootNode->addChildNode(m_node2);
	
	m_playerNode->AddComponent(new Player(true, m_input, m_cameraGripNode));
	m_playerNode->AddComponent(new UIManager(m_2DText));
	m_node2->AddComponent(new Enemy(true));
	m_cameraGripNode->AddComponent(new CameraControl(m_camera, m_playerNode, m_cameraNode, m_input));
}

void Level1::Update()
{
	Level::Update();
	m_particleGenerator->Draw(&m_view, &m_projection, new XMFLOAT3(m_camera->GetX(), m_camera->GetY(), m_camera->GetZ()));
}