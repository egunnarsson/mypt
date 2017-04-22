
#include "AABox.h"

AABox::AABox()
{
	m_bounds[MIN] = Position(0);
	m_bounds[MAX] = Position(0);
}

AABox::AABox(const AABox &other)
{
	m_bounds[MIN] = other.m_bounds[MIN];
	m_bounds[MAX] = other.m_bounds[MAX];
}
AABox::AABox(Position p0, Position p1)
{
	//m_p0 = vec3(min(p0.x, p1.x), min(p0.y, p1.y), min(p0.x, p1.z));
	//m_p1 = vec3(max(p0.x, p1.x), max(p0.y, p1.y), max(p0.x, p1.z));

	m_bounds[MIN] = vmin(p0, p1);
	m_bounds[MAX] = vmax(p0, p1);

	/*m_size = p0 - p1;
	m_size.x = abs(m_size.x) * 0.5;
	m_size.y = abs(m_size.y) * 0.5;
	m_size.z = abs(m_size.z) * 0.5;

	m_position = m_bounds[MIN] + m_size;*/
}

AABox::AABox(const AABox &box0, const AABox &box1)
{
	m_bounds[MIN] = vmin(box0.m_bounds[MIN], box1.m_bounds[MIN]);
	m_bounds[MAX] = vmax(box0.m_bounds[MAX], box1.m_bounds[MAX]);
}

AABox::AABox(const Triangle &tri)
{
	m_bounds[MIN] = vmin(vmin(tri.m_p0, tri.m_p1), tri.m_p2);
	m_bounds[MAX] = vmax(vmax(tri.m_p0, tri.m_p1), tri.m_p2);
}

void AABox::grow(const AABox &other)
{
	m_bounds[MIN] = vmin(m_bounds[MIN], other.m_bounds[MIN]);
	m_bounds[MAX] = vmax(m_bounds[MAX], other.m_bounds[MAX]);
}

Position AABox::getCenter() const
{
	return m_bounds[MIN] + ((m_bounds[MAX] - m_bounds[MIN]) * 0.5);
}

std::pair<AABox, AABox> AABox::split(int axis) const
{
	Position dAxis(0);
	dAxis[axis] = ((m_bounds[MAX][axis] - m_bounds[MIN][axis]) * 0.5);
	return std::make_pair(AABox(m_bounds[MIN], m_bounds[MAX] - dAxis), AABox(m_bounds[MIN] + dAxis, m_bounds[MAX]));
}

number AABox::intersect(const Ray ray) const
{
	const number invDirX = ray.invDirection().x;
	const number invDirY = ray.invDirection().y;
	const number invDirZ = ray.invDirection().z;
	
	const int rSignX = ray.direction().x < 0 ? 1 : 0;
	const int rSignY = ray.direction().y < 0 ? 1 : 0;
	const int rSignZ = ray.direction().z < 0 ? 1 : 0;

	//number tmin, tmax, tymin, tymax, tzmin, tzmax;
	
	number tmin = (m_bounds[rSignX].x - ray.origin().x) * invDirX;
	number tmax = (m_bounds[1 - rSignX].x - ray.origin().x) * invDirX;
	
	const number tymin = (m_bounds[rSignY].y - ray.origin().y) * invDirY;
	const number tymax = (m_bounds[1 - rSignY].y - ray.origin().y) * invDirY;
	if ((tmin > tymax) || (tymin > tmax)) return 0;
	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;

	const number tzmin = (m_bounds[rSignZ].z - ray.origin().z) * invDirZ;
	const number tzmax = (m_bounds[1 - rSignZ].z - ray.origin().z) * invDirZ;
	if ((tmin > tzmax) || (tzmin > tmax)) return 0;
	if (tzmin > tmin) tmin = tzmin;
	//if (tzmax < tmax) tmax = tzmax;
	//if (tmin > r.tmin) r.tmin = tmin;
	//if (tmax < r.tmax) r.tmax = tmax;



	return tmin; // tmin < 0 ?
}

/*
bool AABox::intersect(const Ray ray, vec2 &hits) const
{
	//Direction rInvDir = Direction(1, 1, 1) / ray.direction();
	const number invDirX = ray.invDirection().x; // 1.0 / ray.direction().x;
	const number invDirY = ray.invDirection().y; // 1.0 / ray.direction().y;
	const number invDirZ = ray.invDirection().z; // 1.0 / ray.direction().z;

	const int rSignX = ray.direction().x < 0 ? 1 : 0;
	const int rSignY = ray.direction().y < 0 ? 1 : 0;
	const int rSignZ = ray.direction().z < 0 ? 1 : 0;

	//number tmin, tmax, tymin, tymax, tzmin, tzmax;

	number tmin = (m_bounds[rSignX].x - ray.origin().x) * invDirX;
	number tmax = (m_bounds[1 - rSignX].x - ray.origin().x) * invDirX;

	const number tymin = (m_bounds[rSignY].y - ray.origin().y) * invDirY;
	const number tymax = (m_bounds[1 - rSignY].y - ray.origin().y) * invDirY;
	if ((tmin > tymax) || (tymin > tmax)) return false;
	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;

	const number tzmin = (m_bounds[rSignZ].z - ray.origin().z) * invDirZ;
	const number tzmax = (m_bounds[1 - rSignZ].z - ray.origin().z) * invDirZ;
	if ((tmin > tzmax) || (tzmin > tmax)) return false;
	if (tzmin > tmin) tmin = tzmin;
	if (tzmax < tmax) tmax = tzmax;

	hits[MIN] = tmin;
	hits[MAX] = tmax;
	//if (tmin > r.tmin) r.tmin = tmin;
	//if (tmax < r.tmax) r.tmax = tmax;
	
	return true;
}*/