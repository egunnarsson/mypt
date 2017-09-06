
#include "LoaderDefines.h"
#include "Parsing.h"

#include "Loader.h"

using namespace eobj;

const int Loader::NO_INDEX = std::numeric_limits<int>::max();
const unsigned int Loader::POSITION = 0;
const unsigned int Loader::NORMAL = 1;
const unsigned int Loader::TEX_COORD = 2;

namespace {

    // 0 is not allowed, obj i not 0 indexed
    bool adjust_index(int &i, int count)
    {
        if (i == Loader::NO_INDEX)
        {
            return true;
        }
        if (i < 0)
        {
            i = count - i;
        }
        else
        {
            i--;
        }
        return i >= 0;
    }

    bool adjust_index(ivec3 &idx, int count[3])
    {
        return adjust_index(idx[0], count[0]) && adjust_index(idx[1], count[1]) && adjust_index(idx[2], count[2]);
    }

    // -1 is allowed, vertex above face statement
    bool parse_index(Line &line, ivec3 &out)
    {
        if (skip_whitespace(line))
        {
            if (!parse_int(line, out[Loader::POSITION]))
            {
                return false;
            }
            if (line.get() != '/')
            {
                out = ivec3(out[Loader::POSITION], Loader::NO_INDEX, Loader::NO_INDEX);
                return true;
            }

            if (line.next() == '/')
            {
                out[Loader::TEX_COORD] = Loader::NO_INDEX;
                line.next();
            }
            else
            {
                if (!parse_int(line, out[Loader::TEX_COORD]))
                {
                    return false;
                }
                if (line.get() != '/')
                {
                    return false;
                }
                line.next();
            }

            // legal to skip this tho?
            if (!parse_int(line, out[Loader::NORMAL]))
            {
                return false;
            }

            return true;
        }
        return false;
    }
}

Loader::Loader(const std::string &fileName) :
    m_loaded(false),
    m_stream(fileName),
    m_normalMissing(false),
    m_texCoordMissing(false)
{
}

Loader::~Loader()
{
}

bool Loader::load()
{
    if (m_loaded)
        return true;

    if (!m_stream.is_open()) // print error if file doesn't exists...
        return false;

    assert(m_positions.empty() && m_normals.empty() && m_texCoords.empty() && m_chunks.empty());

    std::vector<std::string> materialLibraries;

    // used for error message
    unsigned int lineNum = 0;

    int smoothGroup = 0;
    //int group;
    Chunk *currentChunk = new Chunk();
    m_chunks.push_back(currentChunk);
    m_materials.insert(std::make_pair("", Material("")));

    bool stop = false;
    Line line;

    for (; !stop && line.readFrom(m_stream); ++lineNum)
    {
        // is there a difference string() string("") string(nullptr)... otherwise case '\0' covers empty
        if (line.empty())
            continue;

        switch (line.get()) {
        case '\0': // ------------- empty line -----
        case '#':  // ---------------- comment -----
        case '\n': // should be consumed by std::getline
            break;

        case 'v':
            switch (line.get(1)) {
            case ' ': // ---- vertex position -----
            case '\t':
                if (!parse<eobj::Position>(line, m_positions))
                    stop = true;
                break;

            case 't': // --------- text-coord ----
                if (!parse<vec2>(line, m_texCoords))
                    stop = true;
                break;

            case 'n': // ------ vertex normal ----
                if (!parse<Direction>(line, m_normals))
                    stop = true;
                break;

            case 'p': // ---------- free form ----
                break;

            default:
                stop = true;
                break;
            }
            break;

        case 'f': // ------------------- face ----
        {
            ++line.m_it; // ++pos;

            ivec3 idx1(NO_INDEX), idx2(NO_INDEX), idx3(NO_INDEX), idx4(NO_INDEX);

            if (!parse_index(line, idx1)) stop = true;
            if (!parse_index(line, idx2)) stop = true;
            if (!parse_index(line, idx3)) stop = true;

            int count[3] = { static_cast<int>(m_positions.size()), static_cast<int>(m_normals.size()), static_cast<int>(m_texCoords.size()) };

            if (!adjust_index(idx1, count)) stop = true;
            if (!adjust_index(idx2, count)) stop = true;
            if (!adjust_index(idx3, count)) stop = true;

            if (stop) break;

            Triangle *t = addFace(currentChunk, smoothGroup, idx1, idx2, idx3);

            m_normalMissing |= (idx1[NORMAL] == NO_INDEX || idx2[NORMAL] == NO_INDEX || idx3[NORMAL] == NO_INDEX);
            m_texCoordMissing |= (idx1[TEX_COORD] == NO_INDEX || idx2[TEX_COORD] == NO_INDEX || idx3[TEX_COORD] == NO_INDEX);

            if (parse_index(line, idx4)) // quad => make two triangles
            {
                Triangle *t2 = addFace(currentChunk, smoothGroup, idx1, idx3, idx4);

                m_normalMissing |= (idx1[NORMAL] == NO_INDEX || idx3[NORMAL] == NO_INDEX || idx4[NORMAL] == NO_INDEX);
                m_texCoordMissing |= (idx1[TEX_COORD] == NO_INDEX || idx3[TEX_COORD] == NO_INDEX || idx4[TEX_COORD] == NO_INDEX);
            }

            break;
        }
        case 'g': // --------------- group -------
            // only for editing tools?
            break;
        case 's': // --------------- smooth ------
            line.m_it++;
            if (skip_whitespace(line))
            {
                if (!parse_int(line, smoothGroup))
                {
                    if (equal("off", line.m_it))
                    {
                        smoothGroup = 0;
                    }
                    else
                    {
                        stop = true;
                    }
                }
            }
            else
            {
                stop = true;
            }
            break;
        // "mg" // --------------- merge group -----
                // only for free form
        case 'u': // --------------- material ----
            if (equal("usemtl", line.m_it)) 
            {
                line.m_it += 6;
                std::string name;
                if (parse_word(line, name))
                {
                    // check if same material has already been used...
                    currentChunk = nullptr;
                    for (Chunk *chunk : m_chunks)
                    {
                        if (chunk->material == name)
                        {
                            currentChunk = chunk;
                            break;
                        }
                    }

                    //... otherwise add new chunk
                    if (currentChunk == nullptr) 
                    {
                        currentChunk = new Chunk();
                        currentChunk->material = name;
                        m_chunks.push_back(currentChunk);
                    }
                }
                else 
                {
                    currentChunk = m_chunks[0];
                }
            }
            else 
            {
                stop = true;
            }
            break;

        case 'm':
            if (equal("mtllib", line.m_it))  //strncmp(pos, "usemtl", 6) == 0) {
            {
                line.m_it += 6;
                std::string mtlLib;
                if (parse_word(line, mtlLib))
                {
                    materialLibraries.push_back(mtlLib);
                }
                else
                {
                    stop = true;
                }
            }
            break;
            /*when everything is implemented we can use this
        default:
            pos = &stop;
            break;

            */
        }

    }

    // normalize smooth normals
    for (Direction &normal : m_smoothNormals)
    {
        normal.normalize();
    }

    // fix smooth normal indices
    int offset = static_cast<int>(m_normals.size());
    auto fixNormalIndex = [offset](int &i) {
        if (i < 0)
        {
            i = offset + (-i);
        }
    };

    for (Chunk *chunk : m_chunks)
    {
        for (Triangle &triangle : chunk->triangles)
        {
            fixNormalIndex(triangle.vertices[0][NORMAL]);
            fixNormalIndex(triangle.vertices[1][NORMAL]);
            fixNormalIndex(triangle.vertices[2][NORMAL]);
        }
    }

    // append smoothed normals to normal array
    m_normals.insert(m_normals.end(), m_smoothNormals.begin(), m_smoothNormals.end());

    if (m_texCoordMissing)
    {
        m_texCoords.push_back(vec2());
        int texCoordIndex = static_cast<int>(m_texCoords.size() - 1);
        for (Chunk *c : m_chunks)
        {
            for (Triangle &t : c->triangles)
            {
                for (int i = 0; i <= 3; ++i)
                {
                    if (t.vertices[i][TEX_COORD] == NO_INDEX)
                    {
                        t.vertices[i][TEX_COORD] = texCoordIndex;
                    }
                }
            }
        }
    }

    for (const std::string &materialLibrary : materialLibraries)
    {
        Material::loadMaterialLibrary(materialLibrary, m_materials);
    }

    // sanity checks? 
    // does materials exist
    // 

    m_loaded = true;
    return true;
}

void Loader::addSmoothNormal(ivec3 &idx, Direction normal, int &faceNormalIndex, int smoothGroup)
{
    if (idx[NORMAL] == NO_INDEX)
    {
        if (smoothGroup == 0)
        {
            if (faceNormalIndex == NO_INDEX)
            {
                m_smoothNormals.push_back(normal);
                faceNormalIndex = static_cast<int>(m_smoothNormals.size() - 1); // we never want 0? what about signed 0...
            }
            idx[NORMAL] = faceNormalIndex;
        }
        else
        {
            auto key = std::make_pair(idx[POSITION], smoothGroup);
            auto it = smoothMap.find(key);
            if (it == smoothMap.end())
            {
                m_smoothNormals[it->second] += normal;
                idx[NORMAL] = it->second;
            }
            else
            {
                m_smoothNormals.push_back(normal);
                idx[NORMAL] = static_cast<int>(m_smoothNormals.size() - 1);
                smoothMap.insert(std::make_pair(key, idx[NORMAL]));
            }
        }
    }
}

Loader::Triangle *Loader::addFace(Chunk *chunk, int smoothGroup, ivec3 idx1, ivec3 idx2, ivec3 idx3)
{
    // these might not be there yet?
    // since -1 is allowed I'm guessing vertex have to come first
    Position v1 = m_positions[idx1[POSITION]];
    Position v2 = m_positions[idx2[POSITION]];
    Position v3 = m_positions[idx3[POSITION]];

    Position u = v2 - v1;
    Position v = v3 - v1;

    Direction normal = Direction::cast(u.cross(v).normalize());
    int faceNormalIndex = NO_INDEX;

    addSmoothNormal(idx1, normal, faceNormalIndex, smoothGroup);
    addSmoothNormal(idx2, normal, faceNormalIndex, smoothGroup);
    addSmoothNormal(idx3, normal, faceNormalIndex, smoothGroup);

    Triangle face;
    face.smoothGroup = smoothGroup;
    face.vertices[0] = idx1;
    face.vertices[1] = idx2;
    face.vertices[2] = idx3;

    chunk->triangles.push_back(face);
    return &chunk->triangles.back();
}

std::size_t Loader::chunkCount() const
{
    return m_chunks.size();
}

const Loader::Chunk* Loader::getChunk(std::size_t i) const
{
    return m_chunks.at(i);
}

std::vector<ivec3> Loader::getChunkIndices(std::size_t i) const
{
    Chunk *c = m_chunks.at(i);
    std::vector<eobj::ivec3> indices;
    indices.reserve(c->triangles.size() * 3);
    for (Triangle t : c->triangles)
    {
        indices.push_back(t.vertices[0]);
        indices.push_back(t.vertices[1]);
        indices.push_back(t.vertices[2]);
    }
    return indices;
}

const std::vector<eobj::Position>& Loader::positions()
{
    return m_positions;
}

const std::vector<Direction>& Loader::normals()
{
    return m_normals;
}

const std::vector<vec2>& Loader::texCoords()
{
    return m_texCoords;
}

bool Loader::hasMissingNormal() const
{
    return m_normalMissing;
}

bool Loader::hasMissingTexCoord() const
{
    return m_texCoordMissing;
}

Material Loader::getMaterial(int i) const
{
    //return m_materials[m_chunks[i]->material];
    return m_materials.at(m_chunks[i]->material);
}