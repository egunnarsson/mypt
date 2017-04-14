
#ifndef RAY_H
#define RAY_H

#include "Defines.h"

class Ray
{
public:

	Ray() {}

	Ray(Position origin, Direction direction) :
		m_origin(origin),
		m_direction(direction)
	{}

	static Ray fromTo(Position from, Position to);

	void calculateInvDir();

	const Position& origin() const { return m_origin; }
	const Direction& direction() const { return m_direction; }
	const Direction& invDirection() const { return m_invDirection; }

	Ray reflect(number t, Direction normal) const;
	Ray refract(number t, Direction normal, number eta) const;

	Ray& nudge();

	Position operator()(number t) const;

private:

	Position  m_origin;
	Direction m_direction;
	Direction m_invDirection;
};

#endif