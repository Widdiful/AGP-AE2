#pragma once
#include "Vector3.h"

struct Plane {
	Vector3 normal;
	float d;
};

class Maths
{
public:
	Maths();
	~Maths();
	float dot(Vector3 v1, Vector3 v2);
	Vector3 cross(Vector3 v1, Vector3 v2);
	Vector3 normal(Vector3 v1, Vector3 v2, Vector3 v3);
	float magnitude(Vector3 v1);
	Vector3 normalise(Vector3 v1);
	Plane CalculatePlane(Vector3 v1, Vector3 v2, Vector3 v3);
	float PlaneEquation(Plane p, Vector3 v);
	Vector3 planeIntersection(Plane* p, Vector3* v1, Vector3* v2);
	static float clamp(float value, float min, float max);
};

