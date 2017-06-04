
#ifndef AABVH_H
#define AABVH_H

#include <vector>
#include <queue>

#include <tbb\atomic.h>
#include <tbb\cache_aligned_allocator.h>

#include "Ray.h"
#include "AABox.h"
#include "Triangle.h"

class Mesh;
class Scene;

class AABVH
{
public:

	struct HitInfo
	{
		number t;
		const Mesh *mesh;
		const Triangle *triangle;
		uint_fast32_t chunkIndex;
		uint_fast32_t triangleIndex;
	};

	struct Statistics 
	{
		tbb::atomic<uint64_t> rayCount;
		tbb::atomic<uint64_t> triCount;
		tbb::atomic<uint64_t> boxCount;
	};

	AABVH();
	AABVH(const Scene *scene);

	bool intersect(Ray ray, HitInfo &hit, Statistics &stat) const;

private:

	__declspec(align(16)) struct Element
	{
		Element();
		Element(AABox aabox, Element *left, Element *right);
		Element(const Mesh *mesh, const Triangle *triangle, uint_fast32_t chunkIndex, uint_fast32_t triangleIndex);

        Element(const Element &);
        Element& operator = (const Element &);

		union
		{
			struct 
            {
				AABox m_box;
                Element *m_left;
                Element *m_right;
			};

			struct 
            {
				const Mesh *m_mesh;
				const Triangle *m_triangle;
				uint_fast32_t m_chunkIndex;
				uint_fast32_t m_triangleIndex;
			};
		};

		bool m_isLeaf;

		AABox getBox() const;
	};

	std::vector<Element, tbb::cache_aligned_allocator<Element>> m_elements;

	Element *buildMeshTree(std::vector<sp<const Mesh>> meshes, uint_fast32_t &elementIndex);
    Element *joinElements(std::queue<Element*> &elements, uint_fast32_t &elementIndex);

};

#endif
