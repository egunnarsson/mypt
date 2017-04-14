
#include <stack>
#include <queue>
#include <functional>

#include <tbb\cache_aligned_allocator.h>

#include "AABVH.h"
#include "Scene.h"
#include "Mesh.h"
#include "Triangle.h"

/*AABVH::Element::Element() :
	m_isLeaf(false),
	m_box(),
	right(nullptr),
	left(nullptr),
	m_mesh(nullptr)
{
}*/

AABVH::Element::Element(Triangle *_triangle, uint_fast32_t _chunkIndex, uint_fast32_t _triangleIndex) :
	m_isLeaf(true)
{
	triangle = _triangle;
	chunkIndex = _chunkIndex;
	triangleIndex = _triangleIndex;
}

AABVH::Element::Element(Mesh *mesh, Element *_left, Element *_right) :
	m_box(_left->getBox(), _right->getBox()),
	left(_left),
	right(_right),
	m_mesh(mesh),
	m_isLeaf(false)
{
	assert(mesh != nullptr);
	assert(_left != nullptr);
	assert(_right != nullptr);
}

AABVH::Element::Element(Element *_left, Element *_right) :
	m_box(_left->getBox(), _right->getBox()),
	left(_left),
	right(_right),
	m_mesh(nullptr),
	m_isLeaf(false)
{
	assert(_left != nullptr);
	assert(_right != nullptr);
}

AABVH::Element::~Element()
{
	/*if (!m_isLeaf && m_mesh == nullptr)
	{
		if (!left->m_isLeaf && left->m_mesh)
			delete[] left;
		else
			delete left;
		if (!right->m_isLeaf && right->m_mesh)
			delete[] right;
		else
			delete right;
	}*/
	delete left;
	delete right;
}

AABox AABVH::Element::getBox()
{
	if (m_isLeaf)
		return AABox(*triangle);
	else
		return m_box;
}

AABVH::AABVH() :
	m_root(nullptr)
{
}

AABVH::Element* AABVH::buildMeshTree(sp<Mesh> meshPtr) const 
{
	//std::queue<Element*> queue;
	
	std::vector<Element*> queue;

	// pack all triangles into elements with pairs of two triangles
	uint_fast32_t chunkIndex = 0;
	for (auto chunkIt = meshPtr->m_chunks.begin(); chunkIt < meshPtr->m_chunks.end(); ++chunkIt)
	{
		uint_fast32_t triangleIndex = 0;
		for (auto triIt = (*chunkIt).m_triangles.begin(); triIt < (*chunkIt).m_triangles.end(); ++triIt)
		{
			/*if (!queue.empty() && queue.back()->right.leaf.triangle == nullptr)
			{
				Element *elem = queue.back();

				LeafData &leaf = elem->right.leaf;
				leaf.triangle = &(*triIt);
				leaf.chunkIndex = chunkIndex;
				leaf.triangleIndex = triangleIndex;

				elem->m_box = AABox(elem->m_box, AABox(*triIt));
			}
			else*/
			{
				queue.push_back(new Element(&(*triIt), chunkIndex, triangleIndex));
			}
			++triangleIndex;
		}
		++chunkIndex;
	}

	printf("triangles %d\n", queue.size());
	//Element *skipList = new Element[queue.size() + queue.size() - 1];
	//tbb::fixed_pool

	int elementCount = 0;

	std::function<Element*(AABox, int, const std::vector<Element*>&)> split;

	split = [&split, &meshPtr, &elementCount](const AABox &bounds, int axis, const std::vector<Element*> &elements) -> Element*
	{
		if (elements.empty())
		{
			return nullptr;
		}
		else if (elements.size() == 1)
		{
			return elements.front();
		}
		else if (elements.size() == 2)
		{
			elementCount++;
			return new Element(meshPtr.get(), elements.front(), elements.back());
		}
		else
		{
			std::vector<Element*> left;
			std::vector<Element*> right;

			number border = bounds.getCenter()[axis];

			for (Element *e : elements)
			{
				if (e->getBox().getCenter()[axis] < border)
				{
					left.push_back(e);
				}
				else
				{
					right.push_back(e);
				}
			}

			std::pair<AABox, AABox> newBoxes = bounds.split(axis);
			int newAxis = (axis + 1) % 3;

			Element *rightElement = split(newBoxes.first, newAxis, left);
			Element *leftElement = split(newBoxes.second, newAxis, right);

			if (rightElement == nullptr)
				return leftElement;
			else if (leftElement == nullptr)
				return rightElement;
			else 
			{
				elementCount++;
				return new Element(meshPtr.get(), rightElement, leftElement);
			}
		}

	};

	Element *meshElement = split(meshPtr->getBoundingBox(), 0, queue);
	if (meshElement->m_isLeaf) 
	{
		elementCount++;
		meshElement = new Element(meshElement, nullptr); // this nullptr will cause a crash...
	}
	meshElement->m_mesh = meshPtr.get();
	
	printf("elemnts %d\n", elementCount + queue.size());

	return meshElement;
}

AABVH::AABVH(const Scene *scene) :
	m_root(nullptr)
{
	std::queue<Element* /*, std::vector<Element*> */> meshBoxes;
	const size_t count = scene->meshCount();
	for (size_t i = 0; i < count; ++i)
	{
		sp<Mesh> meshPtr = scene->getMesh(i);
		Element *meshElement = buildMeshTree(meshPtr);

		if (meshElement)
			meshBoxes.push(meshElement);
	}

	// build tree structure of mesh elements
	while (meshBoxes.size() >= 2)
	{
		Element *left = meshBoxes.front(); meshBoxes.pop();
		Element *right = meshBoxes.front(); meshBoxes.pop();
		meshBoxes.push(new Element(left, right));
	}

	if (!meshBoxes.empty())
		m_root = meshBoxes.front();
}

AABVH::~AABVH()
{
	delete m_root;
}

bool AABVH::intersect(Ray ray, HitInfo &hit, Statistics &stats) const
{
	hit.t = 0;
	hit.mesh = nullptr;
	hit.triangle = nullptr;
	hit.chunkIndex = 0;
	hit.triangleIndex = 0;

	if (m_root == nullptr)
		return false;

	number t = NUM_INF;

	Mesh *currentMesh = nullptr;
	Mesh *hitMesh = nullptr;
	Element *hitElement = nullptr;

	uint64_t triCount = 0;
	uint64_t boxCount = 0;

	std::vector<Element*, tbb::cache_aligned_allocator<Element*> > stack;
	stack.push_back(m_root);

	auto intersectElement = [&](Element *element) -> number {
		if (element->m_isLeaf)
		{
			number hit = element->triangle->intersect(ray); ++triCount;
			if (hit && hit < t)
			{
				t = hit;
				hitMesh = currentMesh;
				hitElement = element;
			}
			return 0;
		}
		else
		{
			++boxCount;
			number hit = element->m_box.intersect(ray);
			return hit > t ? 0 : hit;
		}
	};

	while (!stack.empty())
	{
		Element &current = *stack.back();
		stack.pop_back();

		if (current.m_mesh)
		{
			currentMesh = current.m_mesh;
		}
		
		number leftHit = intersectElement(current.left);
		number rightHit = intersectElement(current.right);

		if (leftHit)
		{
			if (rightHit)
			{
				if (leftHit < rightHit)
				{
					stack.push_back(current.right);
					stack.push_back(current.left);
				}
				else
				{
					stack.push_back(current.left);
					stack.push_back(current.right);
				}
			}
			else
			{
				stack.push_back(current.left);
			}
		}
		else if (rightHit)
		{
			stack.push_back(current.right);
		}
	}

	/*while (!stack.empty())
	{
		Element &current = *stack.back();
		stack.pop_back();

		if (current.m_mesh)
		{
			currentMesh = current.m_mesh;
		}

		if (current.m_leafChildren)
		{
			number leftHit = current.left.leaf.triangle->intersect(ray); ++triCount;

			if (leftHit && leftHit < t)
			{
				t = leftHit;
				hitMesh = currentMesh;
				hitLeaf = &current.left.leaf;
			}

			if (current.right.leaf.triangle)
			{
				number rightHit = current.right.leaf.triangle->intersect(ray); ++triCount;

				if (rightHit && rightHit < t)
				{
					t = rightHit;
					hitMesh = currentMesh;
					hitLeaf = &current.right.leaf;
				}
			}
		}
		else
		{
			/ *if (current.m_mesh)
			{
				currentMesh = current.m_mesh;
			}* /

			number leftHit = current.left.element->m_box.intersect(ray); ++boxCount;
			number rightHit = current.right.element->m_box.intersect(ray); ++boxCount;

			leftHit = leftHit > t ? 0 : leftHit;
			rightHit = rightHit > t ? 0 : rightHit;

			if (leftHit)
			{
				if (rightHit)
				{
					if (leftHit < rightHit)
					{
						stack.push_back(current.right.element);
						stack.push_back(current.left.element);
					}
					else
					{
						stack.push_back(current.left.element);
						stack.push_back(current.right.element);
					}
				}
				else
				{
					stack.push_back(current.left.element);
				}
			}
			else if (rightHit)
			{
				stack.push_back(current.right.element);
			}
		}
	}*/

	stats.rayCount++;
	stats.boxCount += boxCount;
	stats.triCount += triCount;

	if (hitElement)
	{
		hit.t = t;
		hit.mesh = hitMesh;
		hit.triangle = hitElement->triangle;
		hit.chunkIndex = hitElement->chunkIndex;
		hit.triangleIndex = hitElement->triangleIndex;
		return true;
	}

	return false;
}
