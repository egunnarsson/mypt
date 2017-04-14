
#include "Ray.h"

/*
reflect n v = (s *& n) &- v where 
  s = 2 * (n &. v)
*/

Ray Ray::fromTo(Position from, Position to)
{
	return Ray(from, Direction::cast(to - from).normalize());
}

Ray Ray::reflect(number t, Direction normal) const
{
	Position position = (*this)(t);
	number NdotV  = normal.dot(m_direction);
	return Ray(position, m_direction - (normal * (NdotV * 2)));
}

/*
refract' eta n v = 
  if k<0
    then reflect' u i 
    else toNormalUnsafe $ ((-eta) *& v) &- (- eta*c + sqrt k) *& n 
  where
    c = n &. v
    k = 1 - eta*eta*(1-c*c)
*/

Ray Ray::refract(number t, Direction normal, number eta) const
{
	Position position = (*this)(t);
	number NdotV      = normal.dot(-m_direction);
	number k          = 1 - eta * eta * ( 1 - NdotV * NdotV);

	if (k < 0)
		return reflect(t, normal);
	else
		return Ray(position, ((-m_direction) * -eta) - normal * (-eta * NdotV + sqrt(k)));
}

Ray& Ray::nudge()
{
	m_origin += Position::cast(m_direction * NUM_EPS);
	return *this;
}

Position Ray::operator()(number t) const
{
	return m_origin + Position::cast(m_direction * t);
}

void Ray::calculateInvDir()
{
	m_invDirection = Direction(1.0 / m_direction.x, 1.0 / m_direction.y, 1.0 / m_direction.z);
}