
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Defines.h"
#include "Ray.h"

class Triangle
{
public:

	Triangle() /*: m_normal(1, 0, 0)*/ {}
	Triangle(Position p1, Position p2, Position p3);

	Triangle make(Position p1, Position p2, Position p3);

	number intersect(Ray ray) const;
	
	//Direction m_normal; // geometry normal
	Position m_p0, m_p1, m_p2;
	Direction m_e1, m_e2;

	void transform(mat4 matrix);
	Direction calculateNormal() const;
};

#endif