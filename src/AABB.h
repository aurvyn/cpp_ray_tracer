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

	virtual std::array<bool, N> packetIntersect(RayPacket const & rays, Hitpoint* hits) const
	{
		return this->_packetIntersect<true>(rays, hits);
	}
	
	std::array<bool, N> packetIntersectNoUpdate(RayPacket const & rays, Hitpoint* const hits) const
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
	std::array<bool, N> _packetIntersect(RayPacket const & rays, Hitpoint* const hits) const
	{	
		return _packetFastIntersect(rays, hits);
	}

	std::array<bool, N> _packetFastIntersect(RayPacket const & rays, Hitpoint* const hits) const
	{	
		std::array<bool, N> rets;
		for (int i = 0; i < N; i++){
			float tmin, tmax, tymin, tymax;
			float xInv = rays.getInvDirections()[i][0];
			float xOr = rays.getOrigin()[0];
	
			tmin = (bbMin[0] - xOr) * xInv;
			tmax = (bbMax[0]- xOr) * xInv;
			if(tmax < tmin)
					std::swap(tmax, tmin);
	
			if (tmin > hits[i].getParameter() || tmax < 0)
				rets[i] = false;
				continue;
			
			float yInv = rays.getInvDirections()[i][1];
			float yOr = rays.getOrigin()[1];
	
			tymin = (bbMin[1] - yOr) * yInv;
			tymax = (bbMax[1]- yOr) * yInv;
			if(tymax < tymin)
					std::swap(tymax, tymin);
	
			if ( (tmin > tymax) || (tymin > tmax)){
				rets[i] = false;
				continue;
			}
			 
			tmin = tymin > tmin ? tymin : tmin;
			tmax = tymax < tmax ? tymax : tmax;
	
			yInv = rays.getInvDirections()[i][2];
			yOr = rays.getOrigin()[2];
	
			tymin = (bbMin[2] - yOr) * yInv;
			tymax = (bbMax[2]- yOr) * yInv;
			if(tymax < tymin)
					std::swap(tymax, tymin);
	
			if ( (tmin > tymax) || (tymin > tmax)){
				rets[i] = false;
				continue;
			}
			tmin = tymin > tmin ? tymin : tmin;
			tmax = tymax < tmax ? tymax : tmax;
	
			rets[i] = true;	
		}
		return rets;
	}
	
	template<bool updateHit>
	bool _intersect(Ray const & ray, Hitpoint & hit) const
	{	
		return _fastIntersect(ray, hit);
		//we want to find the farthest entrace and closest exit to the box
		//if the exit is closer than the entrance, there is no hit
		// const size_t vecDim = 3;
		// float entrance = 0.0f;
		// float exit = hit.getParameter();
		// Vector3 normal = Vector3(0,0,0);
		
		// for(int i=0; i<vecDim; i++)
		// {
		// 	float slabA = bbMin[i];
		// 	float slabB = bbMax[i];
		// 	float invDir = 1.0f / ray.getDirection()[i];
		// 	float origin = ray.getOrigin()[i];
			
		// 	float closestHit = (slabA - origin) * invDir;
		// 	float farthestHit = (slabB - origin) * invDir;
			
		// 	if(farthestHit < closestHit)
		// 		std::swap(closestHit, farthestHit);
			
		// 	bool tooClose = farthestHit < entrance;
		// 	bool tooFar = closestHit > exit;
			
		// 	if(tooClose || tooFar)
		// 		return false;
			
		// 	bool foundNewEntrance = closestHit > entrance;
		// 	entrance = foundNewEntrance ? closestHit : entrance;
			
		// 	bool foundNewExit = farthestHit < exit;
		// 	exit = foundNewExit ? farthestHit : exit;
			
		// 	if(updateHit) {
		// 		if(foundNewEntrance)
		// 		{
		// 			normal = Vector3(0,0,0);
		// 			normal[i] = ray.getDirection()[i] * -1;
		// 			normal.normalize();
		// 		}
		// 	}
		// }
		
		// if(updateHit) {
		// 	hit.setMaterialId( this->getMaterialId() );
		// 	hit.setNormal(normal);
		// 	hit.setParameter(entrance);
		// }
		
		// return true;
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

