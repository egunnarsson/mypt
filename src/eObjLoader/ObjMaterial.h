
#ifndef OBJ_MATERIAL_H
#define OBJ_MATERIAL_H

#include <string>
#include <map>

#include "LoaderDefines.h"

namespace eobj 
{
    enum IlluminationModel : unsigned int {
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
    };

    class Material
    {

    public:

        static void loadMaterialLibrary(const std::string &fileName, std::map<std::string, Material> &materials);

        Material(std::string name);

        std::string name;

        eobj::Color ambient; // Ka
        eobj::Color diffuse; // Kd
        eobj::Color specular; // Ks
        eobj::Color emission; // Ke

        eobj::Color transmission; // Tf

        IlluminationModel illumination; // illum 

        // transparent
        float dissolve; // d
        bool halo; // d -halo

        // 0 to 1000
        float specularExponent; // Ns

        // reflection exponent... 0 to 1000 (60 default)
        float sharpness; // sharpness

        // index of refraction 0.001 to 10 (1 default, 1.5 glass)
        float density; // Ni

        // maps...

    };
}

#endif