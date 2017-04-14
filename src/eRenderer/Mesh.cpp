
#include "Mesh.h"

void Mesh::setMaterial(Material material)
{
	for (size_t i = 0; i < m_materials.size(); ++i)
	{
		m_materials[i] = material;
	}
}

/*void Mesh::getShadingData(const Triangle *tri, const Material *&material, Direction &normal, vec2 &textureCoordinate) const
{
	auto index = m_triangleMap.find(tri);
	if (index != m_triangleMap.end())
	{
		material = &m_materials.at(index->second);
		normal = m_chunks[index->second].
	}
}*/

/*void Mesh::calculateAccelerationStructure()
{
	Position min(NUM_INF, NUM_INF, NUM_INF);
	Position max(-NUM_INF, -NUM_INF, -NUM_INF);

	const size_t chunks = m_chunks.size();
	for (size_t c=0; c<chunks; ++c)
	{
		const size_t elements = m_chunks[c].m_triangles.size();
		for (size_t e=0; e<elements; ++e)
		{
			Triangle &tri = m_chunks[c].m_triangles[e];
			min = vmin(vmin(vmin(min, tri.m_p0), tri.m_p1), tri.m_p2);
			max = vmax(vmax(vmax(max, tri.m_p0), tri.m_p1), tri.m_p2);
		}
	}

	m_boundingBox = AABox(min, max);
}*/

/*bool Mesh::intersect(Ray r, number &t, Direction &normal, Material &material) const
{
	if (!m_boundingBox.intersect(r))
		return false;

	t = INFINITY;

	size_t hitChunk, hitElement;

	const size_t chunks = m_chunks.size();
	for (size_t c=0; c<chunks; ++c)
	{
		const size_t elements = m_chunks[c].m_triangles.size();
		for (size_t e=0; e<elements; ++e)
		{
			number t2 = m_chunks[c].m_triangles[e].intersect(r);
			if (t2 && t2 < t)
			{
				t          = t2;
				hitChunk   = c;
				hitElement = e;
			}
		}
	}

	if (t == INFINITY)
		return false;
	
	material = m_materials[hitChunk];
	//element  = m_chunks[hitChunk][hitElement];
	normal = m_chunks[hitChunk].m_normals[hitElement*3];
	
	return true;
}*/

AABox Mesh::getBoundingBox() const
{
	AABox box;

	for (auto it = m_chunks.begin(); it < m_chunks.end(); it++)
	{
		for (auto it2 = it->m_triangles.begin(); it2 < it->m_triangles.end(); it2++)
		{
			box = AABox(*it2);
			break;
		}
	}

	for (auto it = m_chunks.begin(); it < m_chunks.end(); it++)
	{
		for (auto it2 = it->m_triangles.begin(); it2 < it->m_triangles.end(); it2++)
		{
			box.grow(AABox(*it2));
		}
	}

	return box;
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

	//calculateAccelerationStructure();
}

Mesh* Mesh::makeBox(Position position, number width, number height, const Material *material /*= nullptr*/)
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

template<class T, class U>
U convert(T &a)
{
	return U(a.x, a.y, a.z);
}

Mesh* Mesh::makeMesh(const std::vector<vector3<unsigned int, POSITION_TYPE> > &indices, const std::vector<vector3<float, POSITION_TYPE> > &positions, const std::vector<vector3<float, DIRECTION_TYPE> > &normals, const std::vector<vector2<float> > &texCoords)
{
	Mesh *mesh = new Mesh();

	mesh->m_chunks.push_back(Chunk());
	Chunk &c = mesh->m_chunks.back();

	//for (vector3<unsigned int, POSITION_TYPE> index : indices)
	for (auto it = indices.begin(); it != indices.end(); it += 3)
	{
		vector3<unsigned int, POSITION_TYPE> index1 = *it;
		vector3<unsigned int, POSITION_TYPE> index2 = *(it + 1);
		vector3<unsigned int, POSITION_TYPE> index3 = *(it + 2);

		c.addTriangle(
			Position::castT(positions[index1[0]]), Direction::castT(normals[index1[1]]), vec2::castT(texCoords[index1[2]]),
			Position::castT(positions[index2[0]]), Direction::castT(normals[index2[1]]), vec2::castT(texCoords[index2[2]]),
			Position::castT(positions[index3[0]]), Direction::castT(normals[index3[1]]), vec2::castT(texCoords[index3[2]]));
	}

	mesh->m_materials.push_back(Material(DIFFUSE, Color(0), Color(0.5, 0.5, 0.5)));
	//mesh->calculateAccelerationStructure();

	return mesh;
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

Mesh* Mesh::makePlane(Position translation, Scale scale, AxisRotation rotation, const Material *material /*= nullptr*/)
{
	Mesh *mesh = new Mesh();

	mesh->m_chunks.push_back(Chunk());
	Chunk &c = mesh->m_chunks.back();

	mesh->m_materials.push_back(Material(DIFFUSE, Color(0), Color(1, 0, 0)));

	c.m_triangles.push_back(Triangle(Position(-0.5, -0.5, -0.5), Position(0.5, -0.5, -0.5), Position(0.5, -0.5, 0.5)));
	c.m_triangles.push_back(Triangle(Position(-0.5, -0.5, -0.5), Position(0.5, -0.5, 0.5), Position(-0.5, -0.5, 0.5)));

	c.m_normals.insert(c.m_normals.begin(), 6, Direction(0, 1, 0));
	c.m_textureCoordinates.insert(c.m_textureCoordinates.begin(), 6, vec2(0, 0));

	mesh->transform(
		mat4::make_translation(translation) *
		mat4::make_rotation(rotation.axis, toRadians(rotation.angle)) *
		mat4::make_scale(scale));

	if (material != nullptr)
		mesh->setMaterial(*material);

	return mesh;
}