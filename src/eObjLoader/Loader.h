
#ifndef LOADER_H
#define LOADER_H

#include <string>
#include <fstream>
#include <vector>

#include <Tvector2.h>
#include <Tvector3.h>

class eObjLoader
{
public:

	typedef vector3<float, POSITION_TYPE> vec3;
	typedef vector3<float, DIRECTION_TYPE> Direction;
	typedef vector2<float> vec2;
	typedef vector3<unsigned int, POSITION_TYPE> uvec3;

	static const unsigned int NO_INDEX;

	static const unsigned int POSITION; 
	static const unsigned int NORMAL; 
	static const unsigned int TEX_COORD;

	eObjLoader(const std::string &fileName);
	~eObjLoader();

	bool load();

	const std::vector<vec3>& positions();
	const std::vector<Direction>& normals();
	const std::vector<vec2>& texCoords();
	const std::vector<uvec3>& indices();

	bool hasMissingNormal() const;
	bool hasMissingTexCoord() const;

	void calculateMissingNormals();
	void fillMissingTexCoords(vec2 defaultValue);

private:

	bool               m_loaded;
	std::ifstream      m_stream;

	std::vector<vec3>  m_positions;
	std::vector<Direction>  m_normals;
	std::vector<vec2>  m_texCoords;
	std::vector<uvec3> m_indices;

	bool m_normalMissing;
	bool m_texCoordMissing;

};

#endif
