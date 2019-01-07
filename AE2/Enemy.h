#pragma once
#include "Actor.h"
#include <vector>
#include "Vector3.h"

class Enemy :
	public Actor
{
private:
	std::vector<Vector3> m_points;
	int m_currentPointIndex = 0;
	Vector3 m_currentPoint;
	float m_distanceToChange;
	float m_chaseDistance;
	SceneNode* m_player;

public:
	Enemy(bool gravity);
	~Enemy();
	void Start();
	void Update();
	Vector3 GetNextPoint();
};

