
#ifndef LOADER_H
#define LOADER_H

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>

#include <Tvector2.h>
#include <Tvector3.h>

#include "LoaderDefines.h"
#include "ObjMaterial.h"

namespace eobj
{
    class Loader
    {

    public:

        static const int NO_INDEX;

        static const unsigned int POSITION;
        static const unsigned int NORMAL;
        static const unsigned int TEX_COORD;

        struct Triangle {
            //int group; // actually std::vector<int>...
            int smoothGroup; // if not 0 normal points into m_smoothNormals
            ivec3 vertices[3]; //{{position, normal, texcoord}}
        };

        /*
        std::vector<std::set<std::string> > groupCombinations;
        // or
        struct Group {
            std::string name;
            std::set<int> faces;
        };
        */

        struct Chunk {
            std::string material;
            std::vector<Triangle> triangles;
        };

        Loader(const std::string &fileName);
        ~Loader();

        bool load();

        const std::vector<Position>& positions();
        const std::vector<Direction>& normals();
        const std::vector<vec2>& texCoords();

        std::size_t chunkCount() const;
        const Chunk* getChunk(std::size_t i) const;
        std::vector<ivec3> getChunkIndices(std::size_t i) const;

        bool hasMissingNormal() const;
        bool hasMissingTexCoord() const;

        Material getMaterial(int i) const;

    private:

        void addSmoothNormal(ivec3 &idx, Direction normal, int &faceNormalIndex, int smoothGroup);
        Triangle *addFace(Chunk *chunk, int smoothGroup, ivec3, ivec3, ivec3);

        bool                    m_loaded;
        std::ifstream           m_stream;

        std::vector<Position>   m_positions;
        std::vector<Direction>  m_normals;
        std::vector<Direction>  m_smoothNormals;
        std::vector<vec2>       m_texCoords;

        struct pair_hash
        {
        public:
            template<typename T, typename U>
            std::size_t operator()(const std::pair<T, U> &x) const
            {
                // lhs^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
                //std::size_t lhs = std::hash<T>()(x.first);
                //std::size_t rhs = std::hash<T>()(x.second);
                //return lhs ^ rhs + 0xA6C92F5A9E3779B9 + (lhs << 6) + (lhs >> 2);
                //return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
                return std::hash<T>()(x.first) * 3 + std::hash<U>()(x.second);
            }
        };

        // vertex index + smooth group -> generated normal index !!!!
        std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int, pair_hash> smoothMap;

        std::vector<Chunk*> m_chunks;

        std::map<std::string, Material> m_materials;

        bool m_normalMissing;
        bool m_texCoordMissing;

    };
}

#endif
