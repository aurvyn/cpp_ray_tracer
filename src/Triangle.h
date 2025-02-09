#ifndef __TRIANGLE
#define __TRIANGLE

#include "Ray.h"
#include "Hitpoint.h"
#include "Primitive.h"

class Triangle : public Primitive
{
public:
	Triangle() { }
	Triangle(Vector3 vertexA, Vector3 vertexB, Vector3 vertexC, Vector3 motion = Vector3(1,0,0))
	{
		this->vertex[0] = vertexA;
		this->vertex[1] = vertexB;
		this->vertex[2] = vertexC;
		this->normal = (vertexB - vertexA).cross(vertexC - vertexB).normalize();
		this->normal = (vertexB - vertexA).cross(vertexC - vertexA).normalize();
		this->motion = motion;
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
		hit.setMotion(this->motion);
		hit.setMaterialId( this->getMaterialId());
		return true;
	}
	
	void setMaterialId(size_t materialId) { this->materialId = materialId; }
	size_t getMaterialId() const { return this->materialId; }
	
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
	
private:
	Vector3 vertex[3];
	Vector3 normal;
	Vector3 motion;
	size_t materialId;
};

#endif

