#pragma once
#include "Level.h"
#include "ParticleGenerator.h"
#include "text2D.h"

class Level1 :
	public Level
{
private:
	SceneNode* m_playerNode;
	SceneNode* m_node2;

	Model* m_model;
	Model* m_model1;

	ParticleGenerator* m_particleGenerator;

	Text2D* m_2DText;

public:
	Level1(InputManager* input, ID3D11Device* device, ID3D11DeviceContext* context, Skybox* skybox);
	~Level1();
	void InitialiseLevel();
	void Update();
};

