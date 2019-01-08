#pragma once
#include "Level.h"
#include "ParticleGenerator.h"
#include "text2D.h"

class Level1 :
	public Level
{
private:
	SceneNode* m_playerNode;
	SceneNode* m_enemyNode;
	SceneNode* m_Box1;

	vector<SceneNode*> m_coins;
	vector<SceneNode*> m_redCoins;

	Model* m_playerModel;
	Model* m_enemyModel;
	Model* m_Box1Model;
	vector<Model*> m_coinModels;
	vector<Model*> m_redCoinModels;

	ParticleGenerator* m_particleGenerator;

	Text2D* m_2DText;

public:
	Level1(InputManager* input, ID3D11Device* device, ID3D11DeviceContext* context, Skybox* skybox);
	~Level1();
	void InitialiseLevel();
	void Update();
	void Restart();
	void CleanUp();
};

