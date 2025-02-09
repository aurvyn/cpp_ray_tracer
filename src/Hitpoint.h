#ifndef __HITPOINT
#define __HITPOINT

#include <limits>

class Hitpoint
{
public:
	Hitpoint()
	{
		this->t = std::numeric_limits<float>::max();
	}

	float getParameter() const { return t; }
	Vector3 getNormal() const { return normal; }
	Vector3 getMotion() const { return motion; }
	size_t getMaterialId() const { return materialId; }

	void setParameter(float const & t) { this->t = t; }
	void setNormal(Vector3 normal) { this->normal = normal; }
	void setMotion(Vector3 motion) { this->motion = motion; }
	void setMaterialId(size_t materialId) {this->materialId = materialId; }

private:
	float t;
	Vector3 normal;
	Vector3 motion;
	size_t materialId;
};
#endif

