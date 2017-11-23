
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "rng.h"

class IMaterial
{
public:

    // brdf, out... importance is part of integrator not material
    struct SampleResult {
        SampleResult(Ray _out,
        Color _reflectance,
        Color _emission) : out(_out), reflectance(_reflectance), emission(_emission) {}

        Ray out;
        Color reflectance; // brdf
        Color emission;
    };

    // sample returns Ray and reflection amount as color from that direction
    // also need emission
    // bool isEmissive() const; ?
    // Color emission(Ray out) const; // emission along out direction

    // Ray, reflectance, emission

    virtual SampleResult sample(Ray ray, number t, Direction normal, rng<number> &rand) const = 0;
};

class SmallPtMaterial : public IMaterial
{
public:
    SmallPtMaterial() : m_emission(0), m_color(0) {};
    SmallPtMaterial(Color emission, Color color);

    virtual SampleResult sample(Ray ray, number t, Direction normal, rng<number> &rand) const override = 0;

    Color      m_emission;
    Color      m_color;
};

class DiffuseMaterial : public SmallPtMaterial
{
public:
    DiffuseMaterial(Color emission, Color color) : SmallPtMaterial(emission, color) {}
    virtual SampleResult sample(Ray ray, number t, Direction normal, rng<number> &rand) const override;
};

class MirrorMaterial : public SmallPtMaterial
{
public:
    MirrorMaterial(Color emission, Color color) : SmallPtMaterial(emission, color) {}
    virtual SampleResult sample(Ray ray, number t, Direction normal, rng<number> &rand) const override;
};

class GlassMaterial : public SmallPtMaterial
{
public:
    GlassMaterial(Color emission, Color color) : SmallPtMaterial(emission, color) {}
    virtual SampleResult sample(Ray ray, number t, Direction normal, rng<number> &rand) const override;
};

class ObjMaterial : public IMaterial
{
public:
    ObjMaterial() {}
    virtual SampleResult sample(Ray ray, number t, Direction normal, rng<number> &rand) const override;

    Color ambient; // Ka
    Color diffuse; // Kd
    Color specular; // Ks
    Color emission; // Ke

    Color transmission; // Tf

    /*
        I0 = 0, // Color on and Ambient off

        I1 = 1, // Color on and Ambient on

        I2 = 2, // Highlight on

        I3 = 3, // Reflection on and Ray trace on

        I4 = 4, // Transparency: Glass on
                //Reflection : Ray trace on

        I5 = 5, // Reflection : Fresnel on and Ray trace on

        I6 = 6, // Transparency : Refraction on
                //Reflection : Fresnel off and Ray trace on

        I7 = 7, // Transparency : Refraction on
                //Reflection : Fresnel on and Ray trace on

        I8 = 8, // Reflection on and Ray trace off

        I9 = 9, // Transparency : Glass on
                //Reflection : Ray trace off

        I10 = 10, // Casts shadows onto invisible surfaces
    */

    unsigned int illumination; // illum 

    // transparent
    float dissolve; // d
    bool halo; // d -halo

    // 0 to 1000
    float specularExponent; // Ns

    // reflection exponent... 0 to 1000 (60 default)
    float sharpness; // sharpness

    // index of refraction 0.001 to 10 (1 default, 1.5 glass)
    float density; // Ni
};

#endif
