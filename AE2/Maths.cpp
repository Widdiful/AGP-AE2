#include "Maths.h"
#include <math.h>


Maths::Maths()
{
}


Maths::~Maths()
{
}

float Maths::dot(Vector3 v1, Vector3 v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

Vector3 Maths::cross(Vector3 v1, Vector3 v2)
{
	float x = (v1.y * v2.z) - (v1.z * v2.y);
	float y = (v1.z * v2.x) - (v1.x * v2.z);
	float z = (v1.x * v2.y) - (v1.y * v2.x);
	return Vector3(x, y, z);
}

Vector3 Maths::normal(Vector3 v1, Vector3 v2, Vector3 v3)
{
	Vector3 a = Vector3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
	Vector3 b = Vector3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
	Vector3 c = cross(a, b);
	return normalise(c);
}

float Maths::magnitude(Vector3 v1)
{
	return sqrt(v1.x*v1.x + v1.y * v1.y + v1.z * v1.z);
}

Vector3 Maths::normalise(Vector3 v1)
{
	float mag = magnitude(v1);
	return Vector3(v1.x / mag, v1.y / mag, v1.z / mag);
}

Plane Maths::CalculatePlane(Vector3 v1, Vector3 v2, Vector3 v3)
{
	Plane plane;

	plane.normal = normal(v1, v2, v3);
	plane.d = -dot(v1, plane.normal);

	return plane;
}

float Maths::PlaneEquation(Plane p, Vector3 v)
{
	return (p.normal.x * v.x) + (p.normal.y * v.y) + (p.normal.z * v.z) + p.d;
}

Vector3 Maths::planeIntersection(Plane * p, Vector3 * v1, Vector3 * v2)
{
	return Vector3();
}
