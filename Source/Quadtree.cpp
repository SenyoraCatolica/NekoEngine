#include "Quadtree.h"

#define NE 0
#define NW 1
#define SE 2
#define SW 3

// QuadtreeNode --------------------------------------------------
QuadtreeNode::QuadtreeNode(const math::AABB& boundingBox) :boundingBox(boundingBox) 
{
	for (uint i = 0; i < 4; ++i)
		children[i] = nullptr;
}

QuadtreeNode::~QuadtreeNode() 
{
	if (!IsLeaf())
	{
		for (uint i = 0; i < 4; ++i)
			RELEASE(children[i]);
	}
}

bool QuadtreeNode::IsLeaf() const
{
	return children[0] == nullptr;
}

void QuadtreeNode::Insert(GameObject* gameObject)
{
	if (objects.size() == BUCKET_SIZE)
	{
		if (IsLeaf())
			Subdivide();

		objects.push_back(gameObject);
		RedistributeChildren();
	}
	else
		objects.push_back(gameObject);
}

void QuadtreeNode::Subdivide()
{
	const math::float3 size = boundingBox.Size();
	const math::float3 halfSize(size.x / 2.0f, size.y, size.z / 2.0f);
	const math::float3 quarterSize(halfSize.x / 2.0f, size.y, halfSize.z / 2.0f);

	const math::float3 center = boundingBox.CenterPoint();
	math::float3 quarterCenter;

	math::AABB quarterBoundingBox;

	// North East
	quarterCenter = { center.x + quarterSize.x, center.y, center.z - quarterSize.z };
	quarterBoundingBox.SetFromCenterAndSize(quarterCenter, halfSize);
	children[NE] = new QuadtreeNode(quarterBoundingBox);

	// North West
	quarterCenter = { center.x - quarterSize.x, center.y, center.z - quarterSize.z };
	quarterBoundingBox.SetFromCenterAndSize(quarterCenter, halfSize);
	children[NW] = new QuadtreeNode(quarterBoundingBox);

	// South East
	quarterCenter = { center.x + quarterSize.x, center.y, center.z + quarterSize.z };
	quarterBoundingBox.SetFromCenterAndSize(quarterCenter, halfSize);
	children[SE] = new QuadtreeNode(quarterBoundingBox);

	// South West
	quarterCenter = { center.x - quarterSize.x, center.y, center.z + quarterSize.z };
	quarterBoundingBox.SetFromCenterAndSize(quarterCenter, halfSize);
	children[SW] = new QuadtreeNode(quarterBoundingBox);
}

void QuadtreeNode::RedistributeChildren()
{
	std::list<GameObject*>::const_iterator it = objects.begin();

	while (it != objects.end())
	{
		uint intersections = 0;
		uint lastIntersection = 0;
		for (uint i = 0; i < 4; ++i)
		{
			if ((*it)->boundingBox.Intersects(children[i]->boundingBox))
			{
				++intersections;
				lastIntersection = i;
			}
		}

		if (intersections == 1)
		{
			children[lastIntersection]->Insert(*it);
			it = objects.erase(it);
		}
		else
			// Any time an object straddles the boundary between two quads, it needs to be stored at the parent level
			++it;
	}
}

template<typename Type>
void QuadtreeNode::CollectIntersections(std::vector<GameObject*>& gameObjects, Type& primitive)
{
	if (primitive.Intersects(boundingBox))
	{
		for (std::list<GameObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
		{
			if (primitive.Intersects((*it)->boundingBox))
				gameObjects.push_back(*it);
		}

		if (!IsLeaf())
		{
			for (uint i = 0; i < 4; ++i)
				children[i]->CollectIntersections(gameObjects, primitive);
		}
	}
}

// Quadtree --------------------------------------------------
Quadtree::Quadtree() {}

Quadtree::~Quadtree() 
{
	Clear();
}

void Quadtree::SetBoundary(const math::AABB& limits)
{
	Clear();
	root = new QuadtreeNode(limits);
}

void Quadtree::Clear()
{
	RELEASE(root);
}

void Quadtree::Insert(GameObject* gameObject)
{
	if (gameObject->boundingBox.Intersects(root->boundingBox))
		root->Insert(gameObject);
}

template<typename Type>
void Quadtree::CollectIntersections(std::vector<GameObject*>& gameObjects, Type& primitive)
{
	if (root != nullptr)
		root->CollectIntersections(gameObjects, primitive);
}