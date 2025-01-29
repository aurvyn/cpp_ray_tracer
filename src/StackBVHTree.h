#ifndef __STACKBVHTREE
#define __STACKBVHTREE

#include <stack>
#include <iostream>
#include <assert.h>
#include "PrimitiveArray.h"
#include "SpatialMedian.h"
#include "BVHTree.h"
template <class T>
class Stack
{
public: 

	Stack(){
		currIndex = 0;
	}

	void push(const T* val){
		arr[currIndex] = val;
		currIndex++;
	}

	const T* pop(){
		currIndex--;
		return arr[currIndex];
	}

	bool hasNext(){
		return currIndex > 0;
	}

private:

	int currIndex;
	const T* arr[100];

};

class StackBVHTree : public BVHTree
{
public:
	
private:
	
	virtual bool traverse(Ray const & ray, Hitpoint & hitpoint, BVHNode const & node) const
	{	
		Stack<BVHNode> stack;
		bool hitOne = false;
		const BVHNode* currentNodePtr = &node;
		while(true){
			if (currentNodePtr != nullptr){
				const BVHNode& currentNode = *currentNodePtr;
				bool hit = currentNode.intersectNoUpdate(ray, hitpoint);
				
				if (hit){
					if(currentNode.isLeaf())
					{
						hitOne = currentNode.getPrimitve()->intersect(ray, hitpoint) || hitOne;
					}else {
						currentNodePtr = currentNode.getLeft();
						stack.push(currentNode.getRight());
						continue;
					}
				}
			}
			if (stack.hasNext()) {
				currentNodePtr = stack.pop();
			} else {
				break;
			}
		}
		return hitOne;
	}
};

#endif

