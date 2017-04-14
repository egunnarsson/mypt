
#include <math.h>

#include "rng.h"

#include "Material.h"

Material::Material(Reflection reflection, Color emission, Color color) :
	m_reflection(reflection),
	m_emission(emission),
	m_color(color)
{
}

inline number fresnel(Direction dir, Direction normal)
{
	const number r0 = 0.04;
	const number c = 1.0 + (dir.dot(normal));
	const number c2 = c * c;
	const number c5 = c2 * c2 * c;
	return r0 + (1.0 - r0) * c5;
}

Ray Material::sample(Ray ray, number t, Direction normal, number &importance, rng<number> &rand) const
{
	switch(m_reflection) {
	case DIFFUSE:
	{
		number
			r1 = 2 * M_PI * rand(),
			r2 = rand(),
			r2s = sqrt(r2);
		Direction
			//w = nl,
			w = normal.dot(ray.direction()) < 0 ? normal : -normal,
			// tanget, binormal
			u = ((fabs(w.x) > 0.1 ? Direction(0, 1, 0) : Direction(1, 0, 0)).cross(w)).normalize(),
			v = w.cross(u);
		Direction d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalize();

		importance = 1;
		return Ray(ray(t), d);
	}
	case MIRROR:
	{
		importance = 1;
		return ray.reflect(t, normal);
	}
	case GLASS:
	{
		bool out    = normal.dot(ray.direction()) > 0;
		number fres = fresnel(ray.direction(), out ? -normal : normal);
		number p2   = 0.25 + 0.5 * fres;

		if (rand() < p2)
		{
			//totF = totF * (fres / p2); // rr
			importance = (fres / p2);
			return ray.reflect(t, normal);
		}
		else
		{
			//totF = totF * ((1 - fres) / (1 - p2)); // rr
			importance = ((1 - fres) / (1 - p2));
			return  out ? ray.refract(t, -normal, 1.5) : ray.refract(t, normal, 1/1.5);
		}
	} }
	return Ray();
}
