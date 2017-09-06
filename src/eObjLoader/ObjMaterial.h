
#ifndef OBJ_MATERIAL_H
#define OBJ_MATERIAL_H

#include <string>
#include <map>

#include "LoaderDefines.h"

namespace eobj 
{
    class Material
    {

    public:

        static void loadMaterialLibrary(const std::string &fileName, std::map<std::string, Material> &materials);

        Material(std::string name);

        std::string name;

        eobj::Color ambient; // Ka
        eobj::Color diffuse; // Kd
        eobj::Color specular; // Ks

        eobj::Color transmission; // Tf

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

        // maps...

    };
}

#endif