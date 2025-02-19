#ifndef __SPHERE
#define __SPHERE

#include "Ray.h"
#include "Hitpoint.h"
#include "Primitive.h"

class Sphere : public Primitive
{
public:
	Sphere() { }
	Sphere(Vector3 position, float radius)
	{
		this->position = position;
		this->radius = radius;
	}
	
	virtual bool intersect(Ray const & ray, Hitpoint & hit) const
	{
		Vector3 d = ray.getDirection();
		Vector3 e = ray.getOrigin();
		Vector3 c = this->getPosition();
		float r = this->getRadius();
		
		//from:
		//ray = e + t*d
		//sphere = (p-c)^2 -r^2 = 0
		
		//quadratic equation
		//(-b +- sqrt(b^2 - 4ac))  / (2a)
		float A, B, C;
		A = d.dot(d);
		B = 2*d.dot(e-c);
		//C = e.dot(e) -2*e.dot(c) + c.dot(c) - r*r;
		C = (e-c).dot(e-c) - r*r;
		
		float discriminant = B*B - 4.0f*A*C;
		if(discriminant < 0.0f)
			return false;
		
		float t1, t2;
		t1 = (-B + sqrt(discriminant)) / (2.0f*A);
		t2 = (-B - sqrt(discriminant)) / (2.0f*A);
		
		if(t1 < 0.0f && t2 < 0.0f)
			return false;
		
		float closestT;
		if(t2 < 0.0f)
			closestT = t1;
		else if(t1 < 0.0f)
			closestT = t2;
		else if(t1 < t2)
			closestT = t1;
		else
			closestT = t2;
		
		if(closestT < hit.getParameter())
		{
			hit.setParameter(closestT);
			
			Vector3 normal = ray.pointAtParameter(closestT) - c;
			hit.setNormal(normal.normalize());
			hit.setMaterialId( this->getMaterialId());
			return true;
		}
		
		return false;
	}

	virtual std::array<bool, N*M> packetIntersect(RayPacket const & rays, Hitpoint* hits) const
	{	
		std::array<bool, N*M> rets;
		alignas(32) float discriminants[8];
		alignas(32) float closest[8]; 

		__m256 orig_x, orig_y, orig_z;
		__m256 c_x = _mm256_set1_ps(this->getPosition()[0]);
		__m256 c_y = _mm256_set1_ps(this->getPosition()[1]);
		__m256 c_z = _mm256_set1_ps(this->getPosition()[2]);
		__m256 radius = _mm256_set1_ps(this->getRadius());

		__m256 dir_x = _mm256_loadu_ps(rays.getDirections()[0].c);
		__m256 dir_y = _mm256_loadu_ps(rays.getDirections()[1].c);
		__m256 dir_z = _mm256_loadu_ps(rays.getDirections()[2].c);

		orig_x = _mm256_set1_ps(rays.getOrigin()[0]);
		orig_y = _mm256_set1_ps(rays.getOrigin()[1]);
		orig_z = _mm256_set1_ps(rays.getOrigin()[2]);

		__m256 A = _mm256_add_ps(
			_mm256_add_ps(_mm256_mul_ps(dir_x, dir_x), _mm256_mul_ps(dir_y, dir_y)),
			_mm256_mul_ps(dir_z, dir_z));

		__m256 B =
						_mm256_mul_ps(_mm256_set1_ps(2.0f), _mm256_add_ps( _mm256_add_ps(
							_mm256_mul_ps(dir_x, _mm256_sub_ps(orig_x, c_x)),
							_mm256_mul_ps(dir_y, _mm256_sub_ps(orig_y, c_y))
						),
						_mm256_mul_ps(dir_z, _mm256_sub_ps(orig_z, c_z)))); 

		__m256 ec_x = _mm256_sub_ps(orig_x, c_x);
		__m256 ec_y = _mm256_sub_ps(orig_y, c_y);
		__m256 ec_z = _mm256_sub_ps(orig_z, c_z);
		__m256 C = _mm256_sub_ps(_mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(ec_x, ec_x), _mm256_mul_ps(ec_x, ec_x)), _mm256_mul_ps(ec_x, ec_x)), _mm256_mul_ps(radius, radius)); // C = (e-c)^2 - r^2

		__m256 discriminant = _mm256_sub_ps(_mm256_mul_ps(B, B), _mm256_mul_ps(_mm256_set1_ps(4.0f), _mm256_mul_ps(A, C)));

		__m256 mask_discriminant = _mm256_cmp_ps(discriminant, _mm256_set1_ps(0.0f), _CMP_LT_OQ);
		_mm256_storeu_ps(discriminants, discriminant);
	
		__m256 sqrt_discriminant = _mm256_sqrt_ps(discriminant);
		__m256 t1 = _mm256_div_ps(_mm256_sub_ps(sqrt_discriminant, B), _mm256_mul_ps(A, _mm256_set1_ps(2.0f)));
		__m256 t2 = _mm256_div_ps(_mm256_sub_ps(_mm256_mul_ps(B, _mm256_set1_ps(-1.0f)), sqrt_discriminant), _mm256_mul_ps(A, _mm256_set1_ps(2.0f)));

		__m256 mask_valid = _mm256_and_ps(mask_discriminant, _mm256_cmp_ps(t1, _mm256_set1_ps(0.0f), _CMP_GE_OQ));

		__m256 mask_t2_neg = _mm256_cmp_ps(t2, _mm256_set1_ps(0.0f), _CMP_LT_OQ);
		__m256 mask_t1_neg = _mm256_cmp_ps(t1, _mm256_set1_ps(0.0f), _CMP_LT_OQ);

		__m256 closestT = _mm256_blendv_ps(_mm256_set1_ps(-1.0f), t1, _mm256_and_ps(mask_t2_neg, _mm256_xor_ps(mask_t1_neg, _mm256_set1_ps(-1.0f))));

		closestT = _mm256_blendv_ps(closestT, t2, _mm256_and_ps(mask_t1_neg, _mm256_xor_ps(mask_t2_neg, _mm256_set1_ps(-1.0f))));

		__m256 mask_t1_lt_t2 = _mm256_and_ps(_mm256_and_ps(_mm256_cmp_ps(t1, t2, _CMP_LT_OQ), _mm256_xor_ps(mask_t1_neg, _mm256_set1_ps(-1.0f))), _mm256_xor_ps(mask_t2_neg, _mm256_set1_ps(-1.0f)));
		closestT = _mm256_blendv_ps(closestT, t1, mask_t1_lt_t2);

		__m256 mask_t2_lt_t1 = _mm256_and_ps(_mm256_and_ps(_mm256_cmp_ps(t2, t1, _CMP_LT_OQ), _mm256_xor_ps(mask_t1_neg, _mm256_set1_ps(-1.0f))), _mm256_xor_ps(mask_t2_neg, _mm256_set1_ps(-1.0f)));
		closestT = _mm256_blendv_ps(closestT, t2, mask_t2_lt_t1);
	
		_mm256_storeu_ps(closest, closestT);
		alignas(32) float resultArr[8];
		_mm256_storeu_ps(resultArr, _mm256_xor_ps(discriminant, _mm256_set1_ps(-1.0f)));
		for (int i = 0; i < N * M; i++) {
			rets[i] = closest[i] >= 0.0f && (resultArr[i] != 0.0f);
		}

		for (int j = 0; j < N * M; j++) {
			if (rets[j]) {
				float closest_t = closest[j];
				if (closest_t < hits[j].getParameter()) {
					hits[j].setParameter(closest_t);
					Vector3 normal = rays.pointAtParameter(j, closest_t) - this->getPosition();
					hits[j].setNormal(normal.normalize());
					hits[j].setMaterialId(this->getMaterialId());
				}
			}
		}
		return rets;
	}
	
	void setMaterialId(size_t materialId) { this->materialId = materialId; }
	size_t getMaterialId() const { return this->materialId; }
	
	virtual Vector3 getBBMin() const
	{
		Vector3 bound;
		bound = this->position - radius;
		return bound;
	}
	
	virtual Vector3 getBBMax() const
	{
		Vector3 bound;
		bound = this->position + radius;
		return bound;
	}
	
private:
	Vector3 position;
	float radius;
	size_t materialId;
	
	Vector3 getPosition() const
	{ return this->position; }
	
	float getRadius() const
	{ return this->radius; }
};

#endif

