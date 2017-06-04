
#include <stack>
#include <queue>
#include <functional>

#include "Log.h"

#include "AABVH.h"
#include "Scene.h"
#include "Mesh.h"
#include "Triangle.h"

AABVH::Element::Element() :
	m_isLeaf(true),
	m_mesh(nullptr),
	m_triangle(nullptr),
	m_chunkIndex(0),
	m_triangleIndex(0)
{
}

AABVH::Element::Element(AABox aabox, Element *left, Element *right) :
	m_isLeaf(false),
	m_box(aabox),
	m_left(left),
	m_right(right)
{
}

AABVH::Element::Element(const Mesh *mesh, const Triangle *triangle, uint_fast32_t chunkIndex, uint_fast32_t triangleIndex) :
	m_isLeaf(true),
	m_mesh(mesh),
	m_triangle(triangle),
	m_chunkIndex(chunkIndex),
	m_triangleIndex(triangleIndex)
{
}

AABVH::Element::Element(const Element &e)
{
    m_isLeaf = e.m_isLeaf;
    if (e.m_isLeaf)
    {
        m_mesh = e.m_mesh;
        m_triangle = e.m_triangle;
        m_chunkIndex = e.m_chunkIndex;
        m_triangleIndex = e.m_triangleIndex;
    }
    else
    {
        m_box = e.m_box;
        m_left = e.m_left;
        m_right = e.m_right;
    }
}

AABVH::Element& AABVH::Element::operator = (const Element &e)
{
    m_isLeaf = e.m_isLeaf;
    if (e.m_isLeaf)
    {
        m_mesh = e.m_mesh;
        m_triangle = e.m_triangle;
        m_chunkIndex = e.m_chunkIndex;
        m_triangleIndex = e.m_triangleIndex;
    }
    else
    {
        m_box = e.m_box;
        m_left = e.m_left;
        m_right = e.m_right;
    }
    return *this;
}

AABox AABVH::Element::getBox() const
{
	if (m_isLeaf)
		return AABox(*m_triangle);
	else
		return m_box;
}

AABVH::AABVH()
{
}

AABVH::Element *AABVH::buildMeshTree(std::vector<sp<const Mesh>> meshes, uint_fast32_t &elementIndex)
{
    std::vector<Element> triangles;
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        sp<const Mesh> meshPtr = meshes[i];
        uint_fast32_t chunkIndex = 0;
        for (auto chunkIt = meshPtr->chunks().begin(); chunkIt < meshPtr->chunks().end(); ++chunkIt)
        {
            uint_fast32_t triangleIndex = 0;
            for (auto triIt = (*chunkIt).m_triangles.begin(); triIt < (*chunkIt).m_triangles.end(); ++triIt)
            {
                triangles.push_back(Element(meshPtr.get(), &(*triIt), chunkIndex, triangleIndex));
                ++triangleIndex;
            }
            ++chunkIndex;
        }
    }

    auto addElement = [this, &elementIndex](const Element &e) -> Element*
    {
        this->m_elements[elementIndex] = e;
        elementIndex--;
        return &this->m_elements[elementIndex + 1];
    };

    std::function<Element*(int, const std::vector<Element>&)> split;
    split = [this, &addElement, &split](int axis, const std::vector<Element> &elements) -> Element*
    {
        if (elements.empty())
        {
            return nullptr;
        }
        else if (elements.size() == 1)
        {
            return addElement(elements.front());
        }
        else if (elements.size() == 2)
        {
            AABox box(elements[0].getBox(), elements.back().getBox());

            Element *front = addElement(elements.front());
            Element *back = addElement(elements.back());

            return addElement(Element(box, front, back));
        }
        else
        {
            AABox bounds(elements[0].getBox());
            for (Element e : elements)
            {
                bounds.grow(e.getBox());
            }

            for (int loop = 0; loop < 3; loop++)
            {
                std::vector<Element> left;
                std::vector<Element> right;

                number border = bounds.getCenter()[axis];

                for (Element e : elements)
                {
                    if (e.getBox().getCenter()[axis] < border)
                    {
                        left.push_back(e);
                    }
                    else
                    {
                        right.push_back(e);
                    }
                }

                if (left.size() != 0 && right.size() != 0)
                {
                    int newAxis = (axis + 1) % 3;

                    Element *rightElement = split(newAxis, left);
                    Element *leftElement = split(newAxis, right);

                    AABox box(rightElement->getBox(), leftElement->getBox());
                    return addElement(Element(box, rightElement, leftElement));
                }

                axis = (axis + 1) % 3;
            }

            // we're stuck, just split elements at the middle

            std::vector<Element> sorted(elements);
            std::sort(sorted.begin(), sorted.end(), [axis](Element a, Element b) {
                return b.getBox().getCenter()[axis] < a.getBox().getCenter()[axis];
            });

            std::size_t const half_size = sorted.size() / 2;
            std::vector<Element> left(sorted.begin(), sorted.begin() + half_size);
            std::vector<Element> right(sorted.begin() + half_size, sorted.end());

            int newAxis = (axis + 1) % 3;

            Element *rightElement = split(newAxis, left);
            Element *leftElement = split(newAxis, right);

            AABox box(rightElement->getBox(), leftElement->getBox());
            return addElement(Element(box, rightElement, leftElement));
        }
    };

    return split(0, triangles);
}

AABVH::Element *AABVH::joinElements(std::queue<Element*> &elements, uint_fast32_t &elementIndex)
{
    while (elements.size() >= 2)
    {
        Element *left = elements.front(); elements.pop();
        Element *right = elements.front(); elements.pop();

        AABox box(left->getBox(), right->getBox());
        m_elements[elementIndex] = Element(box, left, right);
        elementIndex--;

        elements.push(&this->m_elements[elementIndex + 1]);
    }
    return elements.front();
}

AABVH::AABVH(const Scene *scene)
{
    std::vector<sp<const Mesh>> meshes;
    size_t triangleCount = 0;
    for (size_t i = 0; i < scene->meshCount(); ++i)
    {
        sp<const Mesh> meshPtr = scene->getMesh(i);
        meshes.push_back(meshPtr);
        for (auto chunkIt = meshPtr->chunks().begin(); chunkIt < meshPtr->chunks().end(); ++chunkIt)
        {
            triangleCount += chunkIt->m_triangles.size();
        }
    }

    size_t poolSize = triangleCount + triangleCount - 1;
    m_elements.resize(poolSize);
    uint_fast32_t elementIndex = m_elements.size() - 1;

    std::queue<Element*> elements;

    // find meshes that overlap
    while (!meshes.empty())
    {
        sp<const Mesh> mesh = meshes.back();
        meshes.pop_back();
        AABox box = mesh->getBoundingBox();
        std::vector<sp<const Mesh>> group;
        group.push_back(mesh);
        for (auto it = meshes.begin(); it != meshes.end();)
        {
            if (box.intersect((*it)->getBoundingBox()))
            {
                group.push_back(*it);
                it = meshes.erase(it);
            }
            else
            {
                ++it;
            }
        }
        elements.push(buildMeshTree(group, elementIndex));
    }

    // loop
    // find elements thar overlap
    // if none pick two (with smallest aabox)
    
    joinElements(elements, elementIndex);
}

bool AABVH::intersect(Ray ray, HitInfo &hit, Statistics &stats) const
{
	hit.t = 0;
	hit.mesh = nullptr;
	hit.triangle = nullptr;
	hit.chunkIndex = 0;
	hit.triangleIndex = 0;

	if (m_elements.empty())
		return false;

	number t = NUM_INF;

	const Element *hitElement = nullptr;

	uint64_t triCount = 0;
	uint64_t boxCount = 0;

	std::vector<const Element*, tbb::cache_aligned_allocator<const Element*> > stack;
	stack.push_back(&m_elements[0]);

	auto intersectElement = [&](const Element *element) -> number {
		if (element->m_isLeaf)
		{
			number hit = element->m_triangle->intersect(ray); ++triCount;
			if (hit && hit < t)
			{
				t = hit;
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
		const Element *current = stack.back();
		stack.pop_back();

		number leftHit = intersectElement(current->m_left);
		number rightHit = intersectElement(current->m_right);

		if (leftHit)
		{
			if (rightHit)
			{
				if (leftHit < rightHit)
				{
					stack.push_back(current->m_right);
					stack.push_back(current->m_left);
				}
				else
				{
					stack.push_back(current->m_left);
					stack.push_back(current->m_right);
				}
			}
			else
			{
				stack.push_back(current->m_left);
			}
		}
		else if (rightHit)
		{
			stack.push_back(current->m_right);
		}
	}

	stats.rayCount++;
	stats.boxCount += boxCount;
	stats.triCount += triCount;

	if (hitElement)
	{
		hit.t = t;
		hit.mesh = hitElement->m_mesh;
		hit.triangle = hitElement->m_triangle;
		hit.chunkIndex = hitElement->m_chunkIndex;
		hit.triangleIndex = hitElement->m_triangleIndex;
		return true;
	}

	return false;
}
