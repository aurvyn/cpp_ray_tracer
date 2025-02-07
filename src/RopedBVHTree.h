#ifndef __BVHTREE
#define __BVHTREE

#include <assert.h>
#include "Primitive.h"
#include "PrimitiveArray.h"
#include "SpatialMedian.h"
#include "ObjectMedian.h"
#include <iostream>

class RopedBVHNode : public AABB
{
private:
	bool leafNode;
	void const * left;
	void const * rope;
	
public:
	RopedBVHNode()
	{
		leafNode = false;
		left = NULL;
		rope = NULL;
	}
	
	RopedBVHNode const * getLeft() const
	{
		assert(this->leafNode == false);
		return reinterpret_cast<RopedBVHNode const *>(this->left);
	}
	
	/*BVHNode const * getRight() const
	{
		assert(this->leafNode == false);
		return reinterpret_cast<BVHNode const *>(this->right);
	}*/

    RopedBVHNode const * getRope() const
    {
       // assert(this->leafNode == false);
        return reinterpret_cast<RopedBVHNode const *>(this->rope);
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

    void setChildren(RopedBVHNode const * left, RopedBVHNode const * right)
    {
        this->encompass(*left);
        this->encompass(*right);

        this->left = left;
        this->leafNode = false;
    }

    void setRope(RopedBVHNode const * rope)
    {
        if(rope) {
            this->encompass(*rope);
        }
        
        this->rope = rope;

    }
	
	void setPrimitive(Primitive const * prim)
	{
		this->encompass(*prim);
		
		this->left = prim;
		this->leafNode = true;
	}

};

class RopedBVHTree : public Primitive
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
	RopedBVHNode root;
	
	void buildTree(PrimitiveArray const & primitives, RopedBVHNode * parent, int level, const RopedBVHNode * rope)
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
		
		RopedBVHNode * leftNode = new RopedBVHNode();
		RopedBVHNode * rightNode = new RopedBVHNode();

        parent->setRope(rope);
		
		/* tree vis
		for(int i=0; i<level; i++) printf(" ");
		printf("left %d", level);
		*/
		buildTree(leftPrims, leftNode, level+1, rightNode);
		/* tree vis
		for(int i=0; i<level; i++) printf(" ");
		printf("right %d", level);
		*/
		buildTree(rightPrims, rightNode, level+1, parent->getRope());
		
		parent->setChildren(leftNode, rightNode);
       // Vector3 center = parent->getBBMax();
        //std::cout << center[0] << " " << center[1] << " " << center[2] << std::endl;

	}
	
	/*virtual bool traverse(Ray const & ray, Hitpoint & hitpoint, BVHNode const & node) const
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
	}*/

    virtual bool traverse(Ray const & ray, Hitpoint & hitpoint, RopedBVHNode const & node) const
	{
        bool hit = false;
        const RopedBVHNode* currNode = &node;
        while(currNode != NULL) {
            
            bool hitNode = currNode->intersectNoUpdate(ray, hitpoint);

           /* if (currNode->getRope()) {
                std::cout << "Rope set: " << currNode << " -> " << currNode->getRope() << ": hitNode is " << hitNode << std::endl;
            } else {
                std::cout << "Rope set to NULL for node " << currNode << ": hitNode is " << hitNode << std::endl;
            }

            if(!hitNode && !currNode->getRope()) {
                Vector3 min = currNode->getBBMin();
                Vector3 max = currNode->getBBMax();
                std::cout << min[0] << " " << min[1] << " " << min[2] << std::endl;
                std::cout << max[0] << " " << max[1] << " " << max[2] << std::endl;
            }*/
		
            if(!hitNode) {
                currNode = currNode->getRope();
                continue;
            }

            if(currNode->isLeaf())
            {
                hit = currNode->getPrimitve()->intersect(ray, hitpoint) || hit;
                currNode = currNode->getRope();
            } else 
            {
                currNode = currNode->getLeft();
            }

                
        }
		
		return hit;
	}
};

#endif

