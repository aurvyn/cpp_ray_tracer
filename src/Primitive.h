#ifndef __PRIMITIVE
#define __PRIMITIVE

#include "Ray.h"
#include "Hitpoint.h"
#include "RayPacket.h"
#include <array>
#include <immintrin.h>

class Primitive
{
public:

	virtual bool intersect(Ray const & ray, Hitpoint & hit) const
	{ return false; }

	virtual std::array<bool, N*M> packetIntersect(RayPacket const & rays, Hitpoint* hits) const
	{	
		std::array<bool, N*M> val;
		return val;
	}
	
	virtual Vector3 getBBMin() const = 0;
	virtual Vector3 getBBMax() const = 0;
	Vector3 getDim() const
	{
		Vector3 dim = this->getBBMax() - this->getBBMin();
		return dim;
	}
	Vector3 getCenter() const
	{
		return center;
	}

	// this is kinda bad but i am not sure
	void setCenter()
	{
		center = (this->getDim() / 2) + this->getBBMin();
	}
private:
	// added in center
	Vector3 center;
};

#endif

