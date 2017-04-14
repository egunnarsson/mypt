
#ifndef AABVH_H
#define AABVH_H

#include <vector>

#include <tbb\atomic.h>

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
		Mesh *mesh;
		Triangle *triangle;
		uint_fast32_t chunkIndex;
		uint_fast32_t triangleIndex;
	};

	struct Statistics {
		tbb::atomic<uint64_t> rayCount;
		tbb::atomic<uint64_t> triCount;
		tbb::atomic<uint64_t> boxCount;
	};

	AABVH();
	AABVH(const Scene *scene);
	virtual ~AABVH();

	bool intersect(Ray ray, HitInfo &hit, Statistics &stat) const;

private:
	/*
	struct StackElement {
		bool m_leaf;
		AABox m_box;
	};

	std::vector<StackElement> m_skipList;
	*/
	/*
	struct LeafData {

		Triangle *triangle;
		uint_fast32_t chunkIndex;
		uint_fast32_t triangleIndex;
	};
	*/
	/*struct Element2 {

		int data;

		union {
			AABox box;
			Triangle leaf;
		};

		bool isLeaf() { return data > 0; }
		int id() { return abs(data); }

	};*/

	__declspec(align(16)) struct Element
	{
		//Element();
		Element(Mesh *mesh, Element *left, Element *right);
		Element(Element *left, Element *right);
		Element(Triangle *triangle, uint_fast32_t chunkIndex, uint_fast32_t triangleIndex);
		~Element();

		union
		{
			struct {
				AABox m_box;
				Element *right;
				Element *left;
				Mesh *m_mesh;
			};

			//LeafData leaf;
			struct {
				Triangle *triangle;
				uint_fast32_t chunkIndex;
				uint_fast32_t triangleIndex;
			};
		};

		bool m_isLeaf;

		AABox getBox();

		/*union
		{
			Element *element;
			LeafData leaf;
		} left;

		union
		{
			Element *element;
			LeafData leaf;
		} right;

		Mesh *m_mesh;*/
	};

	Element *m_root;

	Element *buildMeshTree(sp<Mesh> meshPtr) const;

};

#endif
