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

	Hitpoint(float _t, Vector3 _normal, size_t _materialId)
		: t(_t), normal(_normal), materialId(_materialId) { }

	float getParameter() const { return t; }
	Vector3 getNormal() const { return normal; }
	size_t getMaterialId() const { return materialId; }

	void setParameter(float const & t) { this->t = t; }
	void setNormal(Vector3 normal) { this->normal = normal; }
	void setMaterialId(size_t materialId) {this->materialId = materialId; }

private:
	float t;
	Vector3 normal;
	size_t materialId;
};
#endif

