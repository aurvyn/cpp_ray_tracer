#ifndef __PRIMITIVE
#define __PRIMITIVE

#include "Ray.h"
#include "Hitpoint.h"
#include "PrettyLogger.h"

class Primitive
{
public:
	virtual bool intersect(Ray const & ray, Hitpoint & hit) const
	{ return false; }
	
	virtual Vector3 getBBMin() const = 0;
	virtual Vector3 getBBMax() const = 0;
	Vector3 getDim() const
	{
		Vector3 dim = this->getBBMax() - this->getBBMin();
		return dim;
	}
	Vector3 getCenter() const
	{
		Vector3 center = (this->getDim() / 2) + this->getBBMin();
		return center;
	}

	virtual float getSignedDistance(Vector3 rayOrigin, Hitpoint& hitpoint) const { 
		LOG_WARN("The native getSignedDistance() function should never be called");
		return 0.0f;
	}

	virtual float getSignedDistance(Vector3 rayOrigin) const { 
		LOG_WARN("The native getSignedDistance() function should never be called");
		return 0.0f;
	}
	
	virtual Vector3 getSDFNorm(Vector3 rayOrigin) const {
        const float h = 0.0001;
        const Vector3 xyy = Vector3(1, -1, -1);
        const Vector3 yyx = Vector3(-1, -1, 1);
        const Vector3 yxy = Vector3(-1, 1, -1);
        const Vector3 xxx = Vector3(1, 1, 1);
        return (xyy * getSignedDistance(rayOrigin + xyy * h) +
                yyx * getSignedDistance(rayOrigin + yyx * h) +
                yxy * getSignedDistance(rayOrigin + yxy * h) +
                xxx * getSignedDistance(rayOrigin + xxx * h))
                .normalize();
	}

	void setMaterialId(size_t materialId) { this->materialId = materialId; }
	size_t getMaterialId() const { return this->materialId; }

    virtual bool isSDF()const{return false;};
protected:
	size_t materialId = 0;
};

#endif

