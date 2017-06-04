
#include "Mesh.h"
#include "Material.h"
#include "Scene.h"

Scene::Scene() :
	m_dirty(false)
{
	m_bvh = new AABVH();
}

Scene::~Scene()
{
	delete m_bvh;
}

void Scene::addMesh(Mesh *mesh)
{
	m_scene.push_back(sp<Mesh>(mesh));
	m_dirty = true;
}

void Scene::addMesh(sp<Mesh> mesh)
{
	m_scene.push_back(mesh);
	m_dirty = true;
}

sp<const Mesh> Scene::getMesh(size_t index) const
{
    return m_scene[index];
}

/*
sp<Mesh> Scene::getMesh(size_t index)
{
	// todo figure out if mesh changes
}
*/

size_t Scene::meshCount() const
{
	return m_scene.size();
}

AABVH::Statistics Scene::s_traceStats;

bool Scene::intersect(const Ray &r, AABVH::HitInfo &hit) const
{
	return m_bvh->intersect(r, hit, s_traceStats);
}

bool Scene::intersect(const Ray &r, number &t, Material &m, Direction &normal) const
{
	/*t = INFINITY;
	number d;
	//size_t id;
	Direction n;
	Material m2;

	const size_t count = m_scene.size();
	for (size_t i = 0; i < count; ++i)
	{
		if (m_scene[i]->intersect(r, d, n, m2))
		{
			if (d < t)
			{
				t = d;
				//id = i;
				normal = n;
				m = m2;
			}
		}
	}
	return t < INFINITY;*/
	return false;
}

void Scene::calculateAccelerationStructure()
{
	if (m_dirty)
	{
		s_traceStats.boxCount = 0;
		s_traceStats.rayCount = 0;
		s_traceStats.triCount = 0;
		delete m_bvh;
		m_bvh = new AABVH(this);
		m_dirty = false;
	}
}