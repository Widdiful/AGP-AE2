#pragma once
#include "Component.h"
#include "Vector3.h"
#include <vector>
class MovingPlatform :
	public Component
{
private:
	std::vector<Vector3> m_points;
	int m_currentPointIndex = 0;
	Vector3 m_currentPoint;
	float m_distanceToChange;

	float m_moveSpeed;
	float m_movementX, m_movementY, m_movementZ;
public:
	MovingPlatform();
	~MovingPlatform();
	void Start();
	void Update();
	Vector3 GetNextPoint();
	void AddPoint(Vector3 newPoint);
	void AddPoints(std::vector<Vector3> newPoints);
	Vector3 GetMovementInfo();
};

