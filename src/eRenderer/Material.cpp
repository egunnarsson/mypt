
#include <math.h>

#include "rng.h"

#include "Material.h"

SmallPtMaterial::SmallPtMaterial(Color emission, Color color) :
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

IMaterial::SampleResult DiffuseMaterial::sample(Ray ray, number t, Direction normal, rng<number> &rand) const
{
    number
        r1 = 2 * M_PI * rand(),
        r2 = rand(),
        r2s = sqrt(r2);
    Direction
        w = normal.dot(ray.direction()) < 0 ? normal : -normal,
        // tanget, binormal
        u = ((fabs(w.x) > 0.1 ? Direction(0, 1, 0) : Direction(1, 0, 0)).cross(w)).normalize(),
        v = w.cross(u);
    Direction d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalize();

    return SampleResult(Ray(ray(t), d), m_color, m_emission);
}

IMaterial::SampleResult MirrorMaterial::sample(Ray ray, number t, Direction normal, rng<number> &rand) const
{
    return SampleResult(ray.reflect(t, normal), m_color, m_emission);
}

IMaterial::SampleResult GlassMaterial::sample(Ray ray, number t, Direction normal, rng<number> &rand) const
{
    bool out = normal.dot(ray.direction()) > 0;
    number fres = fresnel(ray.direction(), out ? -normal : normal);
    number p2 = 0.25 + 0.5 * fres;

    if (rand() < p2)
    {
        return SampleResult(ray.reflect(t, normal),
            m_color, m_emission);
    }
    else
    {
        return SampleResult(out ? ray.refract(t, -normal, 1.5) : ray.refract(t, normal, 1 / 1.5),
            m_color, m_emission);
    }
}


IMaterial::SampleResult ObjMaterial::sample(Ray ray, number t, Direction normal, rng<number> &rand) const
{
    number
        r1 = 2 * M_PI * rand(),
        r2 = rand(),
        r2s = sqrt(r2);
    Direction
        w = normal.dot(ray.direction()) < 0 ? normal : -normal,
        // tanget, binormal
        u = ((fabs(w.x) > 0.1 ? Direction(0, 1, 0) : Direction(1, 0, 0)).cross(w)).normalize(),
        v = w.cross(u);
    Direction d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalize();

    return SampleResult(Ray(ray(t), d), diffuse, emission);
}