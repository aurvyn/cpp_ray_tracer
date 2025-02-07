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
	void const * rope;
	
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
	
	/*BVHNode const * getRight() const
	{
		assert(this->leafNode == false);
		return reinterpret_cast<BVHNode const *>(this->right);
	}*/

    BVHNode const * getRope() const
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
	
	/*void setChildNodes(BVHNode const * left, BVHNode const * right)
	{
		this->encompass(*left);
		this->encompass(*right);
		
		this->left = left;
		this->right = right;
		this->leafNode = false;
	}*/

    void setPointers(BVHNode const * left, BVHNode const * rope)
    {
        this->encompass(*left);
        this->encompass(*rope);

        this->left = left;
        this->rope = rope;
        this->leafNode = false;
    }

    void setRope(BVHNode const * rope)
    {
        this->encompass(*rope);

        this->rope = rope;
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
		buildTree(*primitives, &root, 0, NULL);
		//this->prims = *prims;
		//this->root.encompass(prims->getBBMin(), prims->getBBMax());
	}
	
	virtual bool intersect(Ray const & ray, Hitpoint & hitpoint) const
	{
		return traverse(ray, hitpoint, root);
	}
	
	virtual Vector3 getBBMin() const
	{ return this->root.getBBMin(); }
	virtual Vector3 getBBMax() const
	{ return this->root.getBBMax(); }
	
private:
	
	PrimitiveArray prims;
	BVHNode root;
	
	void buildTree(PrimitiveArray const & primitives, BVHNode * parent, int level, BVHNode * rope)
	{
		assert(primitives.size() > 0);

		
		if(primitives.size() == 1)
		{
			/* tree vis
			printf(", leaf\n");
			*/
			parent->setPrimitive(primitives[0]);
            parent->setRope(rope);
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
		buildTree(leftPrims, leftNode, level+1, rightNode);
		/* tree vis
		for(int i=0; i<level; i++) printf(" ");
		printf("right %d", level);
		*/
		buildTree(rightPrims, rightNode, level+1, rope);
		
		parent->setPointers(leftNode, rope);
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
};

#endif

