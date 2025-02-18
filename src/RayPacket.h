#ifndef __RAYPACKET
#define __RAYPACKET

#include "GenVector.h"
static const int N = 4;

class RayPacket
{
public:
	RayPacket()
	{
	}

	RayPacket(const Vector3* dirs, const Vector3& origin)
    {
        setOrigin(origin);
        setDirection(dirs);
    }

	void setInvDirection()
	{
		for (int i = 0; i < N*N; i++){
			this->inv_dirs[i] = 1.0f / this->dirs[i];
		}
		
	}

	void setDirection(const Vector3* dirs)
    {
        for (int i = 0; i < N*N; i++) {
            this->dirs[i] = dirs[i];
            this->dirs[i].normalize();
        }
        setInvDirection();
    }

	void setOrigin(Vector3 const & origin)
	{ this->origin = origin; }

	const Vector3* getDirections() const
    { return dirs; }

	const Vector3* getInvDirections() const
	{ return inv_dirs; }

	Vector3 getOrigin() const
	{ return this->origin; }

	const Vector3 pointAtParameter(int i, float const & t) const
	{ return origin + (dirs[i] * t); }

private:
	
	Vector3 origin;
	Vector3 dirs[N*N];
	Vector3 inv_dirs[N*N];
};

#endif

