
#include "Mesh.h"

const std::vector<Chunk>& Mesh::chunks() const
{
    return m_chunks;
}

AABox Mesh::getBoundingBox() const
{
	return m_box;
}

void Mesh::setMaterial(std::shared_ptr<const IMaterial> material)
{
    for (size_t i = 0; i < m_chunks.size(); ++i)
    {
        m_chunks[i].m_material = material;
    }
}

void Mesh::transform(mat4 matrix)
{
	const size_t chunks = m_chunks.size();
	for (size_t c=0; c<chunks; ++c)
	{
		const size_t elements = m_chunks[c].m_triangles.size();
		for (size_t e=0; e<elements; ++e)
		{
			//m_chunks[c][e].m_triangle.transform(matrix);
			//m_chunks[c][e].m_normals[0] = m_chunks[c][e].m_triangle.m_normal;
			//m_chunks[c][e].m_normals[1] = m_chunks[c][e].m_triangle.m_normal;
			//m_chunks[c][e].m_normals[2] = m_chunks[c][e].m_triangle.m_normal;
			m_chunks[c].m_triangles[e].transform(matrix);

			Direction normal = m_chunks[c].m_triangles[e].calculateNormal();

			m_chunks[c].m_normals[e * 3]     = normal;
			m_chunks[c].m_normals[e * 3 + 1] = normal;
			m_chunks[c].m_normals[e * 3 + 2] = normal;
		}
	}

    calculateAABox();
}

void Mesh::calculateAABox()
{
    for (auto it = m_chunks.begin(); it < m_chunks.end(); it++)
    {
        for (auto it2 = it->m_triangles.begin(); it2 < it->m_triangles.end(); it2++)
        {
            m_box = AABox(*it2);
            break;
        }
    }

    for (auto it = m_chunks.begin(); it < m_chunks.end(); it++)
    {
        for (auto it2 = it->m_triangles.begin(); it2 < it->m_triangles.end(); it2++)
        {
            m_box.grow(AABox(*it2));
        }
    }
}

Mesh* Mesh::makeBox(Position position, number width, number height, std::shared_ptr<const IMaterial> material /*= nullptr*/)
{
/*	mesh.m_chunks.push_back(Chunk());
	Chunk &c = mesh.m_chunks[0];

	mesh.m_materials.push_back(Material(DIFFUSE, vec3(), vec3(0.75,0.75,0.75)));

	makePlane(mesh, , , vec3()); // but dont calc structure...
	makePlane(mesh, , , vec3());
	makePlane(mesh, , , vec3());
	makePlane(mesh, , , vec3());
	makePlane(mesh, , , vec3());
	makePlane(mesh, , , vec3());

	*/
	return nullptr;
}

void Mesh::addChunk(std::shared_ptr<const IMaterial> material, const std::vector<vector3<int, POSITION_TYPE> > &indices, const std::vector<vector3<float, POSITION_TYPE> > &positions, const std::vector<vector3<float, DIRECTION_TYPE> > &normals, const std::vector<vector2<float> > &texCoords)
{
	m_chunks.push_back(Chunk());
	Chunk &c = m_chunks.back();
    c.m_material = material;

	//for (vector3<unsigned int, POSITION_TYPE> index : indices)
	for (auto it = indices.begin(); it != indices.end(); it += 3)
	{
		vector3<int, POSITION_TYPE> index1 = *it;
		vector3<int, POSITION_TYPE> index2 = *(it + 1);
		vector3<int, POSITION_TYPE> index3 = *(it + 2);

		c.addTriangle(
			Position::castT(positions[index1[0]]), Direction::castT(normals[index1[1]]), vec2::castT(texCoords[index1[2]]),
			Position::castT(positions[index2[0]]), Direction::castT(normals[index2[1]]), vec2::castT(texCoords[index2[2]]),
			Position::castT(positions[index3[0]]), Direction::castT(normals[index3[1]]), vec2::castT(texCoords[index3[2]]));
	}

    calculateAABox(); // dont do this every time?
}

/*
void Mesh::makePlane(Mesh &mesh, vec3 p1, vec3 p2, vec3 normal)
{
	mesh.m_chunks.push_back(Chunk());
	Chunk &c = mesh.m_chunks.back();

	mesh.m_materials.push_back(Material(DIFFUSE, vec3(), vec3(0.25, 0.75, 0.25)));

	vec3 diag = p2 - p1;
	vec3 diag_dir = diag; diag_dir.normalize();

	vec3 tangent_dir = normal.cross(diag_dir); tangent_dir.normalize();
	vec3 tagent      = tangent_dir * diag.length() * 0.5;

	vec3 center = p1 + diag * 0.5;

	vec3 p3 = center + tagent;
	vec3 p4 = center - tagent;

	c.m_triangles.push_back(Triangle(p1, p3, p2));
	c.m_triangles.push_back(Triangle(p1, p4, p2));

	c.m_normals.insert(c.m_normals.begin(), 6, normal);
	c.m_textureCoordinates.insert(c.m_textureCoordinates.begin(), 6, vec2());

	mesh.calculateAccelerationStructure();
}
*/

Mesh* Mesh::makePlane(Position translation, Scale scale, AxisRotation rotation, std::shared_ptr<const IMaterial> material /*= nullptr*/)
{
	Mesh *mesh = new Mesh();

	mesh->m_chunks.push_back(Chunk());
	Chunk &c = mesh->m_chunks.back();
    if (material != nullptr)
        c.m_material = material;
    else
        c.m_material = std::shared_ptr<const SmallPtMaterial>(new DiffuseMaterial(Color(0), Color(1, 0, 0)));

	c.m_triangles.push_back(Triangle(Position(-0.5, -0.5, -0.5), Position(0.5, -0.5, -0.5), Position(0.5, -0.5, 0.5)));
	c.m_triangles.push_back(Triangle(Position(-0.5, -0.5, -0.5), Position(0.5, -0.5, 0.5), Position(-0.5, -0.5, 0.5)));

	c.m_normals.insert(c.m_normals.begin(), 6, Direction(0, 1, 0));
	c.m_textureCoordinates.insert(c.m_textureCoordinates.begin(), 6, vec2(0, 0));

	mesh->transform(
		mat4::make_translation(translation) *
		mat4::make_rotation(rotation.axis, toRadians(rotation.angle)) *
		mat4::make_scale(scale));


	return mesh;
}