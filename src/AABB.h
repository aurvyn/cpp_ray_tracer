#ifndef __AABB
#define __AABB

#include "Ray.h"
#include "Hitpoint.h"
#include "Primitive.h"
#include <iostream>
#include "RayPacket.h"

class AABB : public Primitive
{
public:
	AABB()
	{
		this->bbMin = Vector3(std::numeric_limits<float>::max());
		this->bbMax = Vector3(-std::numeric_limits<float>::max());
	}
	
	AABB(Vector3 bbMin, Vector3 bbMax)
	{
		this->bbMin = Vector3(std::numeric_limits<float>::max());
		this->bbMax = Vector3(-std::numeric_limits<float>::max());
		
		encompass(bbMin, bbMax);
	}
	
	virtual bool intersect(Ray const & ray, Hitpoint & hit) const
	{
		return this->_intersect<true>(ray, hit);
	}
	
	bool intersectNoUpdate(Ray const & ray, Hitpoint const & hit) const
	{
		return this->_intersect<false>(ray, const_cast<Hitpoint &>(hit));
	}

	virtual std::array<bool, N*M> packetIntersect(RayPacket const & rays, Hitpoint* hits) const
	{
		return this->_packetIntersect<true>(rays, hits);
	}
	
	std::array<bool, N*M> packetIntersectNoUpdate(RayPacket const & rays, Hitpoint* const hits) const
	{
		return this->_packetIntersect<false>(rays, const_cast<Hitpoint*>(hits));
	}
	
	void encompass(Vector3 bbMin, Vector3 bbMax)
	{
		const size_t vecDim = 3;
		
		for(int i=0; i<vecDim; i++)
		{
			if(bbMin[i] < this->bbMin[i])
				this->bbMin[i] = bbMin[i];
			if(bbMax[i] > this->bbMax[i])
				this->bbMax[i] = bbMax[i];
		}
	}
	
	void encompass(Primitive const & p)
	{
		encompass(p.getBBMin(), p.getBBMax());
	}
	
	void setMaterialId(size_t materialId) {  }
	size_t getMaterialId() const { return 0; }
	
	virtual Vector3 getBBMin() const
	{ return this->bbMin; }
	virtual Vector3 getBBMax() const
	{ return this->bbMax; }
	
protected:
	Vector3 bbMin;
	Vector3 bbMax;
	
private:

	template<bool updateHit>
	std::array<bool, N*M> _packetIntersect(RayPacket const & rays, Hitpoint* const hits) const
	{	
		return _packetFastIntersect(rays, hits);
	}

	std::array<bool, N*M> _packetFastIntersect(RayPacket const & rays, Hitpoint* const hits) const
	{	
		std::array<bool, N*M> rets;

		__m256 _bbMin = _mm256_set1_ps(bbMin[0]);
		__m256 _bbMax = _mm256_set1_ps(bbMax[0]);
		__m256 _orig = _mm256_set1_ps(rays.getOrigin()[0]);
		__m256 _invDir = _mm256_loadu_ps(rays.getInvDirections()[0].c);

		__m256 tmin = _mm256_mul_ps(_mm256_sub_ps(_bbMin, _orig), _invDir);
		__m256 tmax = _mm256_mul_ps(_mm256_sub_ps(_bbMax, _orig), _invDir);

		__m256 temp_min = _mm256_min_ps(tmin, tmax);
		__m256 temp_max = _mm256_max_ps(tmin, tmax);
		tmin = temp_min;
		tmax = temp_max;

		__m256 rollingMask;
		__m256 mask = _mm256_cmp_ps( _mm256_set1_ps(0), tmax, _CMP_GE_OQ);
		rollingMask = mask;
		if (_mm256_movemask_ps(mask) == 0xFF) {
			for (int i = 0; i < N * M; i++) {
				rets[i] = false;
			}
			return rets;
		}

		_bbMin = _mm256_set1_ps(bbMin[1]);		
		_bbMax = _mm256_set1_ps(bbMax[1]);
		_orig = _mm256_set1_ps(rays.getOrigin()[1]);
		_invDir = _mm256_loadu_ps(rays.getInvDirections()[1].c);

		__m256 tymin = _mm256_mul_ps(_mm256_sub_ps(_bbMin, _orig), _invDir);
		__m256 tymax = _mm256_mul_ps(_mm256_sub_ps(_bbMax, _orig), _invDir);

		temp_min = _mm256_min_ps(tymin, tymax);
		temp_max = _mm256_max_ps(tymin, tymax);
		tymin = temp_min;
		tymax = temp_max;

		mask = _mm256_or_ps(_mm256_cmp_ps(tmin, tymax, _CMP_GT_OQ), _mm256_cmp_ps(tymin, tmax, _CMP_GT_OQ));
		rollingMask = _mm256_or_ps(mask, rollingMask);
		if (_mm256_movemask_ps(mask) == 0xFF) {
			for (int i = 0; i < N * M; i++) {
				rets[i] = false;
			}
			return rets;
		}

		tmin = _mm256_max_ps(tmin, tymin);
		tmax = _mm256_min_ps(tmax, tymax);

		_bbMin = _mm256_set1_ps(bbMin[2]);
		_bbMax = _mm256_set1_ps(bbMax[2]);
		_orig = _mm256_set1_ps(rays.getOrigin()[2]);
		_invDir = _mm256_loadu_ps(rays.getInvDirections()[2].c);

		tymin = _mm256_mul_ps(_mm256_sub_ps(_bbMin, _orig), _invDir);
		tymax = _mm256_mul_ps(_mm256_sub_ps(_bbMax, _orig), _invDir);
		temp_min = _mm256_min_ps(tymin, tymax);
		temp_max = _mm256_max_ps(tymin, tymax);
		tymin = temp_min;
		tymax = temp_max;

		mask = _mm256_or_ps(_mm256_cmp_ps(tmin, tymax, _CMP_GT_OQ), _mm256_cmp_ps(tymin, tmax, _CMP_GT_OQ));
		rollingMask = _mm256_or_ps(mask, rollingMask);
		if (_mm256_movemask_ps(mask) == 0xFF) {
			for (int i = 0; i < N * M; i++) {
				rets[i] = false;
			}
			return rets;
		}

		tmin = _mm256_max_ps(tmin, tymin);
		tmax = _mm256_min_ps(tmax, tymax);

		__m256 result = _mm256_cmp_ps(tmin, tmax, _CMP_LE_OQ);
		alignas(32) float resultArr[8];
		_mm256_storeu_ps(resultArr, _mm256_and_ps(result, _mm256_xor_ps(rollingMask, _mm256_set1_ps(-1.0f))));
		for (int i = 0; i < N * M; i++) {
			rets[i] = (resultArr[i] != 0.0f);
		}
		return rets;
	}
	
	template<bool updateHit>
	bool _intersect(Ray const & ray, Hitpoint & hit) const
	{	
		return _fastIntersect(ray, hit);
	}

	bool _fastIntersect(Ray const & ray, Hitpoint & hit) const
	{
		float tmin, tmax, tymin, tymax;
		float xInv = ray.getInvDirection()[0];
		float xOr = ray.getOrigin()[0];

		tmin = (bbMin[0] - xOr) * xInv;
		tmax = (bbMax[0]- xOr) * xInv;
		if(tmax < tmin)
				std::swap(tmax, tmin);

		if (tmin > hit.getParameter() || tmax < 0)
			return false;
		
		float yInv = ray.getInvDirection()[1];
		float yOr = ray.getOrigin()[1];

		tymin = (bbMin[1] - yOr) * yInv;
		tymax = (bbMax[1]- yOr) * yInv;
		if(tymax < tymin)
				std::swap(tymax, tymin);

		if ( (tmin > tymax) || (tymin > tmax)) return false;
		tmin = tymin > tmin ? tymin : tmin;
		tmax = tymax < tmax ? tymax : tmax;

		yInv = ray.getInvDirection()[2];
		yOr = ray.getOrigin()[2];

		tymin = (bbMin[2] - yOr) * yInv;
		tymax = (bbMax[2]- yOr) * yInv;
		if(tymax < tymin)
				std::swap(tymax, tymin);

		if ( (tmin > tymax) || (tymin > tmax))
			return false;
		tmin = tymin > tmin ? tymin : tmin;
		tmax = tymax < tmax ? tymax : tmax;

		return true;	
	}
};

#endif

