
#ifndef AABOX_H
#define AABOX_H

#include "Defines.h"
#include "Ray.h"
#include "Triangle.h"

class AABox
{
public:

	AABox();
	AABox(const AABox &other);
	AABox(Position p0, Position p1);
	AABox(const AABox &box0, const AABox &box1);
	AABox(const Triangle &tri);

	void grow(const AABox &other);
	std::pair<AABox, AABox> split(int axis) const;

	number intersect(const Ray ray) const;
	//bool intersect(const Ray ray, vec2 &hits) const;
    bool intersect(AABox) const;
	
	const Position& mmin() const { return m_bounds[MIN]; }
	const Position& mmax() const { return m_bounds[MAX]; }

	Position getCenter() const;
    bool empty() const;

private:

	enum { MIN = 0, MAX = 1 };

	Position m_bounds[2];
};

#endif
