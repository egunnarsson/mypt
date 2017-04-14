
//#include <stdio.h>

#include "Triangle.h"

/*namespace {
	Material s_defaultMaterial(DIFFUSE, vec3(), vec3(0.7, 0.7, 0.7));
};

Triangle::Triangle(Material material, vec3 p0, vec3 p1, vec3 p2) :
	m_material(material),
	m_p0(p0),
	m_p1(p1),
	m_p2(p2),
	m_e1(m_p1 - m_p0),
	m_e2(m_p2 - m_p0),
	m_normal((m_p0 - m_p1).cross(m_p0 - m_p2).normalize())
{
}*/

Triangle::Triangle(Position p0, Position p1, Position p2) :
	m_p0(p0),
	m_p1(p1),
	m_p2(p2),
	m_e1(Direction::cast(m_p1 - m_p0)),
	m_e2(Direction::cast(m_p2 - m_p0))
{
	//calculateNormal();
}

//https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
number Triangle::intersect(Ray ray) const
{
	Direction P, Q, T;
	number det, inv_det, u, v;
	number t;

	//Begin calculating determinant - also used to calculate u parameter
	P = ray.direction().cross(m_e2); // CROSS(P, D, e2);
	//if determinant is near zero, ray lies in plane of triangle
	det = m_e1.dot(P); // det = DOT(e1, P);

	//NOT CULLING
	if (det > -NUM_EPS && det < NUM_EPS)
		return 0;

	inv_det = 1 / det;

	//calculate distance from V1 to ray origin
	T = Direction::cast(ray.origin() - m_p0); // SUB(T, O, V1);

	//Calculate u parameter and test bound
	u = T.dot(P) * inv_det; // u = DOT(T, P) * inv_det;

	//The intersection lies outside of the triangle
	if (u < 0 || u > 1)
		return 0;

	//Prepare to test v parameter
	Q = T.cross(m_e1); // CROSS(Q, T, e1);

	//Calculate V parameter and test bound
	v = ray.direction().dot(Q) * inv_det; // v = DOT(D, Q) * inv_det;
	//The intersection lies outside of the triangle
	if(v < 0 || u + v  > 1)
		return 0;

	t = m_e2.dot(Q) * inv_det; // t = DOT(e2, Q) * inv_det;

	if (t > NUM_EPS) // ray intersection
		return t;

	// No hit, no win
	return 0;
}

Direction Triangle::calculateNormal() const
{
	Direction e1 = m_e1; e1.normalize();
	Direction e2 = m_e2; e2.normalize();
	return e1.cross(e2);
}

void Triangle::transform(mat4 matrix)
{
	vec4 p0 = matrix * vec4(m_p0, 1);
	vec4 p1 = matrix * vec4(m_p1, 1);
	vec4 p2 = matrix * vec4(m_p2, 1);

	m_p0 = Position(p0.x, p0.y, p0.z);
	m_p1 = Position(p1.x, p1.y, p1.z);
	m_p2 = Position(p2.x, p2.y, p2.z);

	m_e1 = Direction::cast(m_p1 - m_p0);
	m_e2 = Direction::cast(m_p2 - m_p0);

	//calculateNormal();
}
