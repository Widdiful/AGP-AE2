#include "Level.h"



Level::Level(InputManager* input, ID3D11Device* device, ID3D11DeviceContext* context, Skybox* skybox)
{
	m_input = input;
	m_pD3DDevice = device;
	m_pImmediateContext = context;
	m_skybox = skybox;

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
}


Level::~Level()
{
	delete m_rootNode;
	m_rootNode = nullptr;

	if (m_pRasterSolid != 0) m_pRasterSolid->Release();
	if (m_pRasterSkybox != 0) m_pRasterSkybox->Release();
	if (m_pDepthWriteSolid != 0) m_pDepthWriteSolid->Release();
	if (m_pDepthWriteSkybox != 0) m_pDepthWriteSkybox->Release();
}

void Level::InitialiseLevel()
{
	// Initialise camera
	m_camera = new Camera(0.0, 0.0, -10, 0.0);
	m_camera->Pitch(-60);

	// Initialise essential nodes
	m_rootNode = new SceneNode("Root");
	m_cameraGripNode = new SceneNode("Camera Grip");
	m_cameraNode = new SceneNode("Camera");
	m_rootNode->addChildNode(m_cameraGripNode);
	m_cameraGripNode->addChildNode(m_cameraNode);
}

void Level::StartComponents()
{
	m_rootNode->StartComponents();
}

void Level::Update()
{
	// Set up matrices
	m_world = XMMatrixTranslation(0, 0, 0);
	m_projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), 640.0 / 480.0, 1.0, 1000.0);
	m_view = m_camera->GetViewMatrix();

	// Render skybox
	m_skybox->DrawSkybox(m_camera, &m_view, &m_projection, m_pRasterSkybox, m_pDepthWriteSkybox, m_pRasterSolid, m_pDepthWriteSolid);

	// Run updates for level objects
	m_rootNode->Update(&m_world, &m_view, &m_projection);
}
