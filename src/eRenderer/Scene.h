
#ifndef SCENE_H
#define SCENE_H

#include "Defines.h"
#include "Ray.h"
#include "AABVH.h"

#include <vector>
#include <memory>

class Mesh;
class Material;

/*struct Intersection // move to mesh...
{
	Ray       ray;
	number    t;
	Material *material;
	Direction normal;
};*/

class Scene
{
public:

	Scene();
	~Scene();

	void addMesh(Mesh *mesh);
	void addMesh(sp<Mesh> mesh);
	
	void removeMesh(size_t index);
	void removeMesh(Mesh *mesh);

	size_t meshCount() const;
    sp<const Mesh> getMesh(size_t index) const;

	//bool intersect(Intersection &intersection) const;
	bool intersect(const Ray &r, number &t, Material &m, Direction &normal) const;
	bool intersect(const Ray &r, AABVH::HitInfo &hit) const;

	void calculateAccelerationStructure();

	static AABVH::Statistics s_traceStats;

private:

	std::vector<sp<Mesh>> m_scene;
	AABVH *m_bvh;
	bool m_dirty;
};

#endif