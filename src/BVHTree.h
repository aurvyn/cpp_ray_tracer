#ifndef __BVHTREE
#define __BVHTREE

#include <assert.h>
#include "Primitive.h"
#include "PrimitiveArray.h"
#include "SpatialMedian.h"
#include "ObjectMedian.h"

class BVHNode : public AABB
{
private:
	bool leafNode;
	void const * left;
	void const * right;
	
public:
	BVHNode()
	{
		leafNode = false;
		left = NULL;
		right = NULL;
	}
	
	BVHNode const * getLeft() const
	{
		assert(this->leafNode == false);
		return reinterpret_cast<BVHNode const *>(this->left);
	}
	
	BVHNode const * getRight() const
	{
		assert(this->leafNode == false);
		return reinterpret_cast<BVHNode const *>(this->right);
	}
	
	Primitive const * getPrimitve() const
	{
		assert(this->leafNode == true);
		return reinterpret_cast<Primitive const *>(this->left);
	}
	
	bool isLeaf() const
	{ return this->leafNode; }
	
	void setChildNodes(BVHNode const * left, BVHNode const * right)
	{
		this->encompass(*left);
		this->encompass(*right);
		
		this->left = left;
		this->right = right;
		this->leafNode = false;
	}
	
	void setPrimitive(Primitive const * prim)
	{
		this->encompass(*prim);
		
		this->left = prim;
		this->leafNode = true;
	}
};

class BVHTree : public Primitive
{
public:
	void setContents(PrimitiveArray const * primitives)
	{
		buildTree(*primitives, &root, 0);
		//this->prims = *prims;
		//this->root.encompass(prims->getBBMin(), prims->getBBMax());
	}
	
	virtual bool intersect(Ray const & ray, Hitpoint & hitpoint) const
	{
		return traverse(ray, hitpoint, root);
	}

	virtual std::array<bool, N*N> packetIntersect(RayPacket const & rays, Hitpoint* hits) const
	{	
		return packetTraverse(rays, hits, root);
	}
	
	virtual Vector3 getBBMin() const
	{ return this->root.getBBMin(); }
	virtual Vector3 getBBMax() const
	{ return this->root.getBBMax(); }
	
private:
	
	PrimitiveArray prims;
	BVHNode root;
	
	void buildTree(PrimitiveArray const & primitives, BVHNode * parent, int level)
	{
		assert(primitives.size() > 0);

		
		if(primitives.size() == 1)
		{
			/* tree vis
			printf(", leaf\n");
			*/
			parent->setPrimitive(primitives[0]);
			return;
		}
		/* tree vis
		printf(", nodes %d", primitives.size());
		*/
		
		ObjectMedian split;
		PrimitiveArray leftPrims, rightPrims;
		split.split(primitives, leftPrims, rightPrims);
		
		bool reducingWorkspace = leftPrims.size() < primitives.size() && rightPrims.size() < primitives.size();
		assert(reducingWorkspace);
		
		BVHNode * leftNode = new BVHNode();
		BVHNode * rightNode = new BVHNode();
		
		/* tree vis
		for(int i=0; i<level; i++) printf(" ");
		printf("left %d", level);
		*/
		buildTree(leftPrims, leftNode, level+1);
		/* tree vis
		for(int i=0; i<level; i++) printf(" ");
		printf("right %d", level);
		*/
		buildTree(rightPrims, rightNode, level+1);
		
		parent->setChildNodes(leftNode, rightNode);
	}
	
	virtual bool traverse(Ray const & ray, Hitpoint & hitpoint, BVHNode const & node) const
	{
		bool hitNode = node.intersectNoUpdate(ray, hitpoint);
		
		if(!hitNode)
			return false;
		
		if(node.isLeaf())
		{
			return node.getPrimitve()->intersect(ray, hitpoint);
		}
		
		bool hitLeft  = traverse(ray, hitpoint, *node.getLeft());
		bool hitRight = traverse(ray, hitpoint, *node.getRight());
		
		return hitLeft || hitRight;
	}

	virtual std::array<bool, N*N> packetTraverse(RayPacket const & rays, Hitpoint* hitpoints, BVHNode const & node) const
	{
		std::array<bool, N*N> hitNodes = node.packetIntersectNoUpdate(rays, hitpoints);
		bool hit = false;
		for (int i = 0; i < N*N; i++) {
			hit = hit || hitNodes[i];
		}
		if (!hit){
			std::array<bool, N*N> none;
			for (int i = 0; i < N*N; i++) {
				none[i] = false;
			}
			return none;
		}
		if(node.isLeaf())
		{
			return node.getPrimitve()->packetIntersect(rays, hitpoints);
		}
		
		std::array<bool, N*N> hitLeft  = packetTraverse(rays, hitpoints, *node.getLeft());
		std::array<bool, N*N> hitRight = packetTraverse(rays, hitpoints, *node.getRight());
		for (int i = 0; i < N*N; i++){
			hitLeft[i] = hitLeft[i] || hitRight[i];
		}
		return hitLeft;
	}
};

#endif

