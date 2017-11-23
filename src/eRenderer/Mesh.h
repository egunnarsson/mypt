
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>

#include "Defines.h"
#include "Triangle.h"
#include "Material.h"
#include "AABox.h"


struct Chunk
{
	std::vector<Triangle>  m_triangles;
	std::vector<Direction> m_normals;
	std::vector<vec2>      m_textureCoordinates;

    std::shared_ptr<const IMaterial> m_material;

	void addTriangle(Position p0, Direction n0, vec2 uv0,
					 Position p1, Direction n1, vec2 uv1,
					 Position p2, Direction n2, vec2 uv2)
	{
		m_triangles.push_back(Triangle(p0, p1, p2));
		m_normals.push_back(n0);
		m_normals.push_back(n1);
		m_normals.push_back(n2);
		m_textureCoordinates.push_back(uv0);
		m_textureCoordinates.push_back(uv1);
		m_textureCoordinates.push_back(uv2);
	}
};

class Mesh
{
public:

    const std::vector<Chunk>& chunks() const;
    AABox getBoundingBox() const;

    void addChunk(std::shared_ptr<const IMaterial> material, const std::vector<vector3<int, POSITION_TYPE> > &indices, const std::vector<vector3<float, POSITION_TYPE> > &positions, const std::vector<vector3<float, DIRECTION_TYPE> > &normals, const std::vector<vector2<float> > &texCoords);
	void setMaterial(std::shared_ptr<const IMaterial> material);

	void transform(mat4 matrix);

	static Mesh* makeBox(Position position, number width, number height, std::shared_ptr<const IMaterial> material = nullptr);
	static Mesh* makePlane(Position p1, Position p2, Position normal, std::shared_ptr<const IMaterial> material = nullptr);
	static Mesh* makePlane(Position translation, vector3<number, SCALE_TYPE> scale, AxisRotation rotation, std::shared_ptr<const IMaterial> material = nullptr);

private:

    void calculateAABox();

    std::vector<Chunk> m_chunks;
    AABox m_box;

};

#endif
