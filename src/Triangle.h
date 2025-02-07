#ifndef __TRIANGLE
#define __TRIANGLE

#define clamp(a,b,v) (v) < (a) ? (a) : ( (v) > (b) ? (b) : (v) )
#include "Ray.h"
#include "Hitpoint.h"
#include "Primitive.h"

class Triangle : public Primitive
{
public:
	Triangle() { }
	Triangle(Vector3 vertexA, Vector3 vertexB, Vector3 vertexC)
	{
		this->vertex[0] = vertexA;
		this->vertex[1] = vertexB;
		this->vertex[2] = vertexC;
		this->normal = (vertexB - vertexA).cross(vertexC - vertexB).normalize();
		this->normal = (vertexB - vertexA).cross(vertexC - vertexA).normalize();
	}
	
	virtual bool intersect(Ray const & ray, Hitpoint & hit) const
	{
		Vector3 e = ray.getOrigin();
		Vector3 d = ray.getDirection();
		Vector3 a = this->vertex[0];
		Vector3 b = this->vertex[1];
		Vector3 c = this->vertex[2];
		Vector3 n = this->normal;
		
		//first, plane intersection
		float numer = (a-e).dot(n);
		float denom = d.dot(n);
		
		bool parallelToPlane = denom == 0.0f;
		if(parallelToPlane)
			return false;
		
		float planeHit = numer / denom;
		bool hitBehindOrigin = planeHit < 0.0f;
		if(hitBehindOrigin)
			return false;
		
		//triangle bounds
		Vector3 ab = (b - a);
		Vector3 bc = (c - b);
		Vector3 ca = (a - c);
		
		Vector3 x = ray.pointAtParameter(planeHit);
		Vector3 ax = x - a;
		Vector3 bx = x - b;
		Vector3 cx = x - c;
		
		Vector3 abSide = ab.cross(ax);
		Vector3 bcSide = bc.cross(bx);
		Vector3 caSide = ca.cross(cx);
		
		float abDir = abSide.dot(n);
		float bcDir = bcSide.dot(n);
		float caDir = caSide.dot(n);
		
		if(abDir < 0.0f)
			return false;
		if(bcDir < 0.0f)
			return false;
		if(caDir < 0.0f)
			return false;
		
		bool isCloser = planeHit < hit.getParameter();
		if(!isCloser)
			return false;
			
		hit.setParameter(planeHit);
		hit.setNormal(this->normal);
		hit.setMaterialId( this->getMaterialId());
		return true;
	}

	virtual Vector3 getBBMin() const
	{
		Vector3 bound = vertex[0];
		
		const size_t vecDim = 3;
		const size_t vertCount = 3;
		
		for(int v=0; v<vertCount; v++)
		{
			for(int dim=0; dim<vecDim; dim++)
			{
				if(vertex[v][dim] < bound[dim])
					bound[dim] = vertex[v][dim];
			}
		}
		return bound;
	}
	
	virtual Vector3 getBBMax() const
	{
		Vector3 bound = vertex[0];
		
		const size_t vecDim = 3;
		const size_t vertCount = 3;
		
		for(int v=0; v<vertCount; v++)
		{
			for(int dim=0; dim<vecDim; dim++)
			{
				if(vertex[v][dim] > bound[dim])
					bound[dim] = vertex[v][dim];
			}
		}
		return bound;
	}

	float getSignedDistance(Vector3 rayOrigin) const {
        Vector3 ba = this->vertex[1] - this->vertex[0]; Vector3 pa = rayOrigin - this->vertex[0];
        Vector3 cb = this->vertex[1] - this->vertex[0]; Vector3 pb = rayOrigin - this->vertex[1];
        Vector3 ac = this->vertex[1] - this->vertex[0]; Vector3 pc = rayOrigin - this->vertex[2];
        Vector3 nor = ba.cross(ac);
        return sqrt(((ba.cross(nor).dot(pa) > 0)+(cb.cross(nor).dot(pb) > 0)+(ac.cross(nor).dot(pc) > 0)< 2.0) ?
                    (std::min(std::min((clamp(0.0f,1.0f,(ba.dot(pa)/ba.squaredLength()))*ba-pa).squaredLength(),
                                       (clamp(0.0f,1.0f,(cb.dot(pb)/cb.squaredLength()))*cb-pb).squaredLength()),
                                       (clamp(0.0f,1.0f,(ac.dot(pc)/ac.squaredLength()))*ac-pc).squaredLength())) :
                    (nor.dot(pa)*nor.dot(pa)/nor.squaredLength()));
	}

	Vector3 getSDFNorm(Vector3 rayOrigin) 
	{
		return this->normal;
	}
	
private:
	Vector3 vertex[3];
	Vector3 normal;
};

#endif

