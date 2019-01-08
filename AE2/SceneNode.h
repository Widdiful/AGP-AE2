#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

#include <iostream>
using namespace std;

#include "Camera.h"
#include "text2D.h"
#include "Model.h"
#include "InputManager.h"
#include "ParticleGenerator.h"
#include "Component.h"
#include "Vector3.h"

class Level;
class SceneNode
{
public:
	enum CollisionType { Sphere, Cube };

private:
	Model* m_pModel;
	vector<SceneNode*> m_children;
	vector<Component*> m_components;
	SceneNode* m_parent;
	Level* m_level;

	string m_name;

	Vector3 m_position, m_rotation, m_scale;
	float m_worldCentreX, m_worldCentreY, m_worldCentreZ;
	float m_worldScale;

	bool m_collisionEnabled = true;
	bool m_enabled = true;

	XMMATRIX m_localWorldMatrix;

	CollisionType m_collisionType;

public:
	SceneNode(string name);
	~SceneNode();

	// Core node features
	void SetModel(Model* model);
	void addChildNode(SceneNode* n);
	bool detatchNode(SceneNode* n);
	void SetParent(SceneNode* n);
	SceneNode* GetParent();
	void SetLevel(Level* n);
	Level* GetLevel();
	void Update(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	string GetName();
	void SetEnabled(bool val);

	// Components
	void AddComponent(Component* component);
	Component* GetComponent(std::string name);
	Component* GetComponentInChildren(std::string name);
	SceneNode* FindNode(string name);
	SceneNode* GetRootNode();
	void StartComponents();
	void DeleteComponents();

	// World
	void LookAt_XZ(float x, float z);
	void LookAt_XYZ(float x, float y, float z);
	void LookAt_XZ(float x, float z, float rot);
	void MoveForward(float distance);
	bool MoveForward(float distance, SceneNode* rootNode);
	bool CheckCollision(SceneNode* compareTree);
	bool CheckCollision(SceneNode* compareTree, SceneNode* objectTreeRoot);
	XMVECTOR GetWorldCentrePosition();
	void UpdateCollisionTree(XMMATRIX* world, float scale);
	void SetCollision(bool val);
	void SetCollisionType(CollisionType val);

	// POSITION INFO

	void SetXPos(float x);
	void SetYPos(float y);
	void SetZPos(float z);
	void SetPos(float x, float y, float z);

	void AddXPos(float x);
	void AddYPos(float y);
	void AddZPos(float z);
	void AddPos(float x, float y, float z);

	bool AddXPos(float x, SceneNode* rootNode);
	bool AddYPos(float y, SceneNode* rootNode);
	bool AddZPos(float z, SceneNode* rootNode);
	bool AddPos(float x, float y, float z, SceneNode* rootNode);

	float GetXPos();
	float GetYPos();
	float GetZPos();

	float GetWorldXPos();
	float GetWorldYPos();
	float GetWorldZPos();

	// ROTATION INFO

	void SetXRot(float x);
	void SetYRot(float y);
	void SetZRot(float z);
	void SetRot(float x, float y, float z);

	void AddXRot(float x);
	void AddYRot(float y);
	void AddZRot(float z);
	void AddRot(float x, float y, float z);

	float GetXRot();
	float GetYRot();
	float GetZRot();

	// SCALE INFO

	void SetXScale(float x);
	void SetYScale(float y);
	void SetZScale(float z);
	void SetScale(float x, float y, float z);

	void AddXScale(float x);
	void AddYScale(float y);
	void AddZScale(float z);
	void AddScale(float x, float y, float z);

	float GetXScale();
	float GetYScale();
	float GetZScale();
};

