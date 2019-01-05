#include "Enemy.h"
#include "scene_node.h"


Enemy::Enemy(bool gravity) : Actor(gravity)
{
	m_name = "Enemy";
	m_jumpVelocity = 0.03f;
	m_moveSpeed = 0.005f;
	m_distanceToChange = 0.1f;

	m_points.push_back(Vector3(0, 0, 0));
	m_points.push_back(Vector3(10, 0, 0));
	m_points.push_back(Vector3(0, 0, 10));
	m_points.push_back(Vector3(10, 0, 10));
	m_points.push_back(Vector3(20, 0, 0));
	m_points.push_back(Vector3(-20, 0, 20));

	m_currentPoint = m_points[m_currentPointIndex];
}


Enemy::~Enemy()
{
}

void Enemy::Update()
{
	m_node->LookAt_XZ(m_currentPoint.x, m_currentPoint.z);
	m_node->MoveForward(m_moveSpeed, m_node->GetRootNode());

	Vector3 pos = Vector3(m_node->GetXPos(), m_node->GetYPos(), m_node->GetZPos());
	float dx = pos.x - m_currentPoint.x;
	float dy = pos.y - m_currentPoint.y;
	float dz = pos.z - m_currentPoint.z;

	// check bounding sphere collision
	float distance = sqrt(dx*dx + dy * dy + dz * dz);

	if (distance <= m_distanceToChange){
		m_currentPoint = GetNextPoint();
	}

}

Vector3 Enemy::GetNextPoint()
{
	m_currentPointIndex = (m_currentPointIndex + 1) % m_points.size();

	return m_points[m_currentPointIndex];
}
