#ifndef __PRIMITVEARRAY
#define __PRIMITVEARRAY

#include "Primitive.h"
#include "Sphere.h"
#include "Triangle.h"
#include "AABB.h"
#include <vector>

class PrimitiveArray : public Primitive, private std::vector<Primitive*>
{
public:
	void add(Primitive * prim)
	{
		this->push_back(prim);
		this->bounds.encompass(prim->getBBMin(), prim->getBBMax());
			count++;
			Vector3 center = prim->getCenter();
			for (size_t i = 0; i < 3; ++i) {
				this->median[i] += (center[i] - this->median[i]) / count;
			}
	}
	
	virtual bool intersect(Ray const & ray, Hitpoint & hitpoint) const
	{
		bool hitSomething = false;
		for(int i=0; i<this->size(); i++)
			hitSomething = this->at(i)->intersect(ray, hitpoint) | hitSomething;
		return hitSomething;
	}

	Vector3 getMedian() const
	{
		return median;
	}
	
	virtual Vector3 getBBMin() const
	{ return this->bounds.getBBMin(); }
	virtual Vector3 getBBMax() const
	{ return this->bounds.getBBMax(); }
	
	using std::vector<Primitive*>::size;
	using std::vector<Primitive*>::push_back;
    using std::vector<Primitive*>::operator[];
    using std::vector<Primitive*>::begin;
    using std::vector<Primitive*>::end;
	
private:
	AABB bounds;
	Vector3 median;
	int count = 0;
};

#endif

