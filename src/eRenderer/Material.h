
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "rng.h"

enum Reflection {DIFFUSE, MIRROR, GLASS};


class Material
{
public:
	Material() : m_reflection(DIFFUSE), m_emission(0), m_color(0) {};
	Material(Reflection reflection, Color emission, Color color);

	Ray sample(Ray ray, number t, Direction normal, number &importance, rng<number> &rand) const;

	Reflection m_reflection;
	Color      m_emission;
	Color      m_color;
};

#endif
