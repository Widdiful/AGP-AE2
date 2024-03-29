#include "Level.h"
#include <math.h>
#include <fstream>
#include <sstream>
#include <iterator>
#include <windows.h>

#include "Player.h"
#include "Enemy.h"
#include "CameraControl.h"
#include "LevelCube.h"
#include "Pickup.h"
#include "MovingPlatform.h"
#include "Time.h"



Level::Level(string file, InputManager* input, ID3D11Device* device, ID3D11DeviceContext* context, Skybox* skybox)
{
	// Set up references
	m_input = input;
	m_pD3DDevice = device;
	m_pImmediateContext = context;
	m_skybox = skybox;

	// Create skybox rasters
	D3D11_RASTERIZER_DESC rasteriser_desc;
	ZeroMemory(&rasteriser_desc, sizeof(rasteriser_desc));

	rasteriser_desc.FillMode = D3D11_FILL_SOLID;
	rasteriser_desc.CullMode = D3D11_CULL_BACK;
	m_pD3DDevice->CreateRasterizerState(&rasteriser_desc, &m_pRasterSolid);

	rasteriser_desc.FillMode = D3D11_FILL_SOLID;
	rasteriser_desc.CullMode = D3D11_CULL_FRONT;
	m_pD3DDevice->CreateRasterizerState(&rasteriser_desc, &m_pRasterSkybox);

	D3D11_DEPTH_STENCIL_DESC stencilDesc;
	ZeroMemory(&stencilDesc, sizeof(stencilDesc));
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	m_pD3DDevice->CreateDepthStencilState(&stencilDesc, &m_pDepthWriteSolid);
	stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_pD3DDevice->CreateDepthStencilState(&stencilDesc, &m_pDepthWriteSkybox);

	// Load level file to string vector
	ifstream levelFile(file);
	string line;
	while (std::getline(levelFile, line))
	{
		levelText.push_back(line + "\n");
	}

	//InitialiseLevel();
}


Level::~Level()
{
	if (m_pRasterSolid != 0) m_pRasterSolid->Release();
	if (m_pRasterSkybox != 0) m_pRasterSkybox->Release();
	if (m_pDepthWriteSolid != 0) m_pDepthWriteSolid->Release();
	if (m_pDepthWriteSkybox != 0) m_pDepthWriteSkybox->Release();
}

void Level::InitialiseLevel()
{
	m_redCoinCount = 0;
	m_coinCount = 0;
	m_enemyCount = 0;
	m_complete = false;

	// Initialise camera
	m_camera = new Camera(0.0, 0.0, -10, 0.0);
	m_chestCamera = new Camera(0.0, 0.0, -10, 0.0);
	m_camera->Pitch(-60);

	// Initialise essentials
	m_rootNode = new SceneNode("Root");
	m_cameraGripNode = new SceneNode("Camera Grip");
	m_cameraNode = new SceneNode("Camera");
	m_rootNode->addChildNode(m_cameraGripNode);
	m_cameraGripNode->addChildNode(m_cameraNode);
	m_rootNode->SetLevel(this);

	m_2DText = new Text2D("assets/font2.png", m_pD3DDevice, m_pImmediateContext);

	// Create level from string
	for (int i = 0; i < levelText.size(); i++) {
		string line = levelText[i];
		if (line.size() > 5) {
			istringstream iss(line);
			vector<string> lineInfo((istream_iterator<string>(iss)), istream_iterator<string>());

			// Add new node
			if (lineInfo[0] == "NODE") {
				m_nodes.push_back(new SceneNode(lineInfo[1]));
				m_rootNode->addChildNode(m_nodes.back());
				if (lineInfo[1] == "Coin") {
					m_nodes.back()->SetModel((char*)"assets/coin.obj", (char*)"assets/coin.bmp", m_pD3DDevice, m_pImmediateContext);
					m_nodes.back()->AddComponent(new Pickup());
					m_coinCount++;
				}
				else if (lineInfo[1] == "RedCoin") {
					m_nodes.back()->SetModel((char*)"assets/redCoin.obj", (char*)"assets/redCoin.bmp", m_pD3DDevice, m_pImmediateContext);
					m_nodes.back()->AddComponent(new Pickup());
					m_particles.push_back(new Particles(ParticleGenerator::COIN_GLOW, m_pD3DDevice, m_pImmediateContext));
					m_nodes.back()->AddComponent(m_particles.back());
					m_redCoinCount++;
				}
				else if (lineInfo[1] == "Chest") {
					m_nodes.back()->SetModel((char*)"assets/chest.obj", (char*)"assets/chest.bmp", m_pD3DDevice, m_pImmediateContext);
					m_nodes.back()->AddComponent(new Pickup());
					m_nodes.back()->SetEnabled(false);
				}
			}

			// Add component to previous node
			else if (lineInfo[0] == "COMPONENT") {
				if (lineInfo[1] == "Player") {
					m_nodes.back()->SetModel((char*)"assets/player.obj", (char*)"assets/player.bmp", m_pD3DDevice, m_pImmediateContext);
					m_nodes.back()->AddComponent(new Player(true, m_input, m_cameraGripNode));
					m_cameraGripNode->AddComponent(new CameraControl(m_camera, m_nodes.back(), m_cameraNode, m_input));
					m_particles.push_back(new Particles(ParticleGenerator::DUST_TRAIL, m_pD3DDevice, m_pImmediateContext));
					m_nodes.back()->AddComponent(m_particles.back());
				}
				else if (lineInfo[1] == "Enemy") {
					m_nodes.back()->SetModel((char*)"assets/enemy.obj", (char*)"assets/redCoin.bmp", m_pD3DDevice, m_pImmediateContext);
					m_nodes.back()->AddComponent(new Enemy(true));
					m_enemyCount++;
				}
				else if (lineInfo[1] == "UIManager") {
					m_nodes.back()->AddComponent(new UIManager(m_2DText));
				}
				else if (lineInfo[1] == "LevelCube") {
					m_nodes.back()->SetModel((char*)"assets/Cube.obj", (char*)"assets/platform.bmp", m_pD3DDevice, m_pImmediateContext);
					m_nodes.back()->AddComponent(new LevelCube());
				}
				else if (lineInfo[1] == "MovingPlatform") {
					m_nodes.back()->AddComponent(new MovingPlatform());
					m_nodes.back()->GetModel()->AddTexture((char*)"assets/platform.bmp", (char*)"assets/dots.bmp");
				}
			}

			else if (lineInfo[0] == "ENEMYPOINT") {
				Vector3 newPoint = Vector3(stof(lineInfo[1]), stof(lineInfo[2]), stof(lineInfo[3]));
				static_cast<Enemy*>(m_nodes.back()->GetComponent("Enemy"))->AddPoint(newPoint);
			}

			else if (lineInfo[0] == "PLATFORMPOINT") {
				Vector3 newPoint = Vector3(stof(lineInfo[1]), stof(lineInfo[2]), stof(lineInfo[3]));
				static_cast<MovingPlatform*>(m_nodes.back()->GetComponent("Moving Platform"))->AddPoint(newPoint);
			}

			else if (lineInfo[0] == "MODEL") {
				m_nodes.back()->SetModel((char*)lineInfo[1].c_str(), (char*)lineInfo[2].c_str(), m_pD3DDevice, m_pImmediateContext);
			}

			// Edit position of previous node
			else if (lineInfo[0] == "POS") {
				m_nodes.back()->SetXPos(stof(lineInfo[1]));
				m_nodes.back()->SetYPos(stof(lineInfo[2]));
				m_nodes.back()->SetZPos(stof(lineInfo[3]));
			}

			// Edit scale of previous node
			else if (lineInfo[0] == "SCALE") {
				m_nodes.back()->SetXScale(stof(lineInfo[1]));
				m_nodes.back()->SetYScale(stof(lineInfo[2]));
				m_nodes.back()->SetZScale(stof(lineInfo[3]));
			}

			// Edit skybox texture
			else if (lineInfo[0] == "SKYBOX") {
				m_skybox->SetTexture(lineInfo[1].c_str());
			}
		}

	}

	SceneNode* chest = m_rootNode->FindNode("Chest");
	Vector3 chestPos = Vector3(chest->GetXPos(), chest->GetYPos(), chest->GetZPos());
	m_chestCamera->SetY(chestPos.y + 10);
	m_chestCamera->LookAt(chestPos.x, chestPos.y, chestPos.z);

	// Run Start() methods of all components
	StartComponents();

	// Set up UI reference
	m_uiManager = m_rootNode->GetComponentInChildren("UI Manager");
}

void Level::StartComponents()
{
	if (m_rootNode) m_rootNode->StartComponents();
}

void Level::Update()
{
	// Set up matrices
	m_world = XMMatrixTranslation(0, 0, 0);
	m_projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), 16.0 / 9.0, 1.0, 1000.0);
	if (m_useChestCamera) {
		m_view = m_chestCamera->GetViewMatrix();
		m_skybox->DrawSkybox(m_chestCamera, &m_view, &m_projection, m_pRasterSkybox, m_pDepthWriteSkybox, m_pRasterSolid, m_pDepthWriteSolid);
		m_chestCamTimer -= Time::getInstance().deltaTime / 1000;
		if (m_chestCamTimer <= 0) {
			m_useChestCamera = false;
		}
	}
	else {
		m_view = m_camera->GetViewMatrix();
		m_skybox->DrawSkybox(m_camera, &m_view, &m_projection, m_pRasterSkybox, m_pDepthWriteSkybox, m_pRasterSolid, m_pDepthWriteSolid);
	}

	// Render skybox

	// Run updates for level objects
	XMVECTOR cam = XMVectorSet(m_camera->GetX(), m_camera->GetY(), m_camera->GetZ(), 0.0);
	m_rootNode->Update(&m_world, &m_view, &m_projection, cam);

	// Update particles
	for (int i = 0; i < m_particles.size(); i++) {
		if (m_particles[i] != nullptr) m_particles[i]->Update(&m_view, &m_projection, XMFLOAT3(m_camera->GetX(), m_camera->GetY(), m_camera->GetZ()));
	}

	// Update UI Manager to display UI on top
	if (m_uiManager) m_uiManager->Update();
}

void Level::Restart()
{
	CleanUp();
	InitialiseLevel();
}

void Level::CleanUp()
{


	m_rootNode->DeleteComponents();

	delete m_camera;
	m_camera = nullptr;
	delete m_chestCamera;
	m_chestCamera = nullptr;
	delete m_rootNode;
	m_rootNode = nullptr;
	delete m_cameraGripNode;
	m_cameraGripNode = nullptr;
	delete m_cameraNode;
	m_cameraNode = nullptr;
	delete m_2DText;
	m_2DText = nullptr;

	for (int i = 0; i < m_nodes.size(); i++) {
		delete m_nodes[i];
	}
	m_nodes.clear();
	m_particles.clear();
}

void Level::CompleteLevel()
{
	m_complete = true;
}

void Level::SpawnChest()
{
	m_rootNode->FindNode("Chest")->SetEnabled(true);
	m_useChestCamera = true;
	m_chestCamTimer = 2;
}

int Level::GetCoinCount()
{
	return m_coinCount + m_enemyCount;
}

int Level::GetRedCoinCount()
{
	return m_redCoinCount;
}

bool Level::IsComplete()
{
	return m_complete;
}

bool Level::IsUsingChestCamera()
{
	return m_useChestCamera;
}
