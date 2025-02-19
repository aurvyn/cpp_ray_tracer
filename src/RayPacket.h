#ifndef __RAYPACKET
#define __RAYPACKET

#include "GenVector.h"
#include <iostream>
static const int N = 4;
static const int M = 2;

class RayPacket
{
public:
	RayPacket()
	{
	}

	RayPacket(const std::vector<Vector8> dirs, const Vector3& origin)
    {
        setOrigin(origin);
        setDirection(dirs);
    }

	void setInvDirection()
	{
		inv_dirs.resize(3);
		for (int i = 0; i < 3; i++){
			this->inv_dirs[i] = 1.0f / this->dirs[i];
		}
		
	}

	void setDirection(const std::vector<Vector8> dirsIn)
    {
		this->dirs.resize(3);
		for (int i = 0; i < 8; i++) {
			float lenSq = dirsIn[0][i] * dirsIn[0][i] + dirsIn[1][i] * dirsIn[1][i] + dirsIn[2][i] * dirsIn[2][i];
			float len = std::sqrt(lenSq);
			
			if (len != 0.0f) {
				this->dirs[0][i] = dirsIn[0][i] / len;
				this->dirs[1][i] = dirsIn[1][i] / len;
				this->dirs[2][i] = dirsIn[2][i] / len;
			}
		}
        setInvDirection();
    }

	void setOrigin(Vector3 const & origin)
	{ this->origin = origin; }

	const std::vector<Vector8> getDirections() const
    { return dirs; }

	const std::vector<Vector8> getInvDirections() const
	{ return inv_dirs; }

	Vector3 getOrigin() const
	{ return this->origin; }

	const Vector3 pointAtParameter(int i, float const & t) const
	{ 
		Vector3 off;
		for (int j = 0; j < 3; j++){
			off[j] = dirs[j][i] * t;
		}
		return origin + off; 
	}

private:
	Vector3 origin;
	std::vector<Vector8> dirs;
	std::vector<Vector8> inv_dirs;
};

#endif

