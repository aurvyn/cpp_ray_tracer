#ifndef __RAY
#define __RAY

#include "GenVector.h"

class Ray
{
public:
	Ray()
	{
	}

	Ray(Vector3 const & dir, Vector3 const & origin)
	{
		setDirection(dir);
		setOrigin(origin);
	}

	void setDirection(Vector3 const & dir)
	{
		this->dir = dir;
		this->dir.normalize();
	}

	void setOrigin(Vector3 const & origin)
	{ this->origin = origin; }

	Vector3 getDirection() const
	{ return this->dir; }

	Vector3 getOrigin() const
	{ return this->origin; }

	Vector3 pointAtParameter(float const & t) const
	{ return origin + dir * t; }

private:
	Vector3 origin;
	Vector3 dir;
};

#endif

