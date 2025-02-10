#ifndef __AABB
#define __AABB

#include "Ray.h"
#include "Hitpoint.h"
#include "Primitive.h"

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
	bool _intersect(Ray const & ray, Hitpoint & hit) const
	{
		//we want to find the farthest entrace and closest exit to the box
		//if the exit is closer than the entrance, there is no hit
		const size_t vecDim = 3;
		float entrance = 0.0f;
		float exit = hit.getParameter();
		Vector3 normal = Vector3(0,0,0);
		
		for(int i=0; i<vecDim; i++)
		{
			float slabA = bbMin[i];
			float slabB = bbMax[i];
			float invDir = 1.0f / ray.getDirection()[i];
			float origin = ray.getOrigin()[i];
			
			float closestHit = (slabA - origin) * invDir;
			float farthestHit = (slabB - origin) * invDir;
			
			if(farthestHit < closestHit)
				std::swap(closestHit, farthestHit);
			
			bool tooClose = farthestHit < entrance;
			bool tooFar = closestHit > exit;
			
			if(tooClose || tooFar)
				return false;
			
			bool foundNewEntrance = closestHit > entrance;
			entrance = foundNewEntrance ? closestHit : entrance;
			
			bool foundNewExit = farthestHit < exit;
			exit = foundNewExit ? farthestHit : exit;
			
			if(updateHit) {
				if(foundNewEntrance)
				{
					normal = Vector3(0,0,0);
					normal[i] = ray.getDirection()[i] * -1;
					normal.normalize();
				}
			}
		}
		
		if(updateHit) {
			hit.setMaterialId( this->getMaterialId() );
			hit.setNormal(normal);
			hit.setParameter(entrance);
		}
		
		return true;
	}
};

#endif

