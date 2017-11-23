
#include <fstream>

#include "Log.h"
#include "Parsing.h"
#include "ObjMaterial.h"

using namespace eobj;

namespace
{
    bool parse_material(Material &material, std::ifstream &file, Line &out, unsigned int &lineNum)
    {
        bool stop = false;
        Line line;
        for (; !stop && line.readFrom(file); lineNum++)
        {
            if (line.empty())
                continue;

            switch (line.get())
            {
            case '#': // comment
                break;
            case 'K':
                switch (line.next())
                {
                case 'a':
                    line.next();
                    stop = !parse_color(line, material.ambient);
                    break;
                case 'd':
                    line.next();
                    stop = !parse_color(line, material.diffuse);
                    break;
                case 's':
                    line.next();
                    stop = !parse_color(line, material.specular);
                    break;
                case 'e':
                    line.next();
                    stop = !parse_color(line, material.emission);
                    break;
                }
                break;
            case 'T':
                if (line.next() == 'f')
                {
                    line.next();
                    stop = !parse_color(line, material.transmission);
                }
                break;
            case 'i':
                if (equal("illum", line.m_it))
                {
                    line.m_it += 5;
                    unsigned int illumination;
                    stop = !parse_uint(line, illumination);
                    material.illumination = static_cast<IlluminationModel>(illumination);
                }
                else
                {
                    stop = true;
                }
                break;
            case 'd':
                line.next();
                if (skip_whitespace(line))
                {
                    if (line.get() == '-')
                    {
                        line.next();
                        if (equal("halo", line.m_it))
                        {
                            line.m_it += 4;
                            material.halo = true;
                        }
                        else
                        {
                            stop = true;
                        }
                    }
                    if (!stop)
                    {
                        stop = !parse_float(line, material.dissolve);
                    }
                }
                break;
            case 'N':
                switch (line.next())
                {
                case 's': // Ns
                {
                    line.next();
                    stop = !parse_float(line, material.specularExponent);
                }
                break;
                case 'i': // Ni
                {
                    line.next();
                    stop = !parse_float(line, material.density);
                }
                break;
                default:
                    stop = true;
                    break;
                }
                break;
            case 's':
                if (equal("sharpness", line.m_it))
                {
                    line.m_it += 9;
                    stop = !parse_float(line, material.sharpness);
                }
                else
                {
                    stop = true;
                }
                break;
            case 'n':
                if (equal("newmtl", line.m_it))
                {
                    stop = true;
                    break;
                }
            default:
                return false;
            }
        }

        out = line;
        out.m_it = out.m_string.begin();
        return true;
    }
}

Material::Material(std::string _name) :
    name(_name),
    ambient(0), // Ka
    diffuse(0.5f, 0.5f, 0.5f), // Kd
    specular(0), // Ks
    emission(0), // Ke
    transmission(0), // Tf
    illumination(I0), // illum 
                     // transparent
    dissolve(1), // d
    halo(false), // d -halo
                 // 0 to 1000
    specularExponent(60), // Ns
                          // reflection exponent... 0 to 1000 (60 default)
    sharpness(60), // sharpness
                   // index of refraction 0.001 to 10 (1 default, 1.5 glass)
    density(1) // Ni
{
}

void Material::loadMaterialLibrary(const std::string &fileName, std::map<std::string, Material> &materials)
{
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        LOG_ERROR("unable to open mtl file " + fileName)
        return;
    }

    // used for error message
    unsigned int lineNum = 0;

    bool stop = false;
    Line line;

    stop = !line.readFrom(file);

    while (!stop)
    {
        if (!line.empty() && equal("newmtl", line.m_it))
        {
            line.m_it += 6;
            std::string name;

            if (parse_word(line, name))
            {
                Material m(name);

                if (!parse_material(m, file, line, lineNum))
                {
                    LOG_ERROR("parse error in mtl file " + fileName + " at line:" + lineNum);
                    return;
                }

                if (materials.find(name) != materials.end())
                {
                    LOG_WARNING("duplicate material definition for " + name + " in mtl file " + fileName);
                }
                else
                {
                    materials.insert(std::make_pair(name, m));
                }
            }
            else
            {
                LOG_ERROR("parse error in mtl file " + fileName + " at line:" + lineNum);
                return;
            }
        }
        else
        {
            if (line.empty() || line.get() == '#')
            {
                stop = !line.readFrom(file);
                ++lineNum;
            }
            else
            {
                LOG_ERROR("parse error in mtl file " + fileName + " at line:" + lineNum);
                return;
            }
        }
    }
}
