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

	virtual std::array<bool, N*N> packetIntersect(RayPacket const & rays, Hitpoint* hits) const
	{	
		std::array<bool, N*N> rets;
		for (int i = 0; i < N*N; i++){
			Vector3 d = rays.getDirections()[i];
			Vector3 e = rays.getOrigin();
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
			if(discriminant < 0.0f){
				rets[i] = false;
				continue;
			}
			
			float t1, t2;
			t1 = (-B + sqrt(discriminant)) / (2.0f*A);
			t2 = (-B - sqrt(discriminant)) / (2.0f*A);
			
			if(t1 < 0.0f && t2 < 0.0f){
				rets[i] = false;
				continue;
			}
			
			float closestT;
			if(t2 < 0.0f)
				closestT = t1;
			else if(t1 < 0.0f)
				closestT = t2;
			else if(t1 < t2)
				closestT = t1;
			else
				closestT = t2;
			
			if(closestT < hits[i].getParameter())
			{
				hits[i].setParameter(closestT);
				
				Vector3 normal = rays.pointAtParameter(i, closestT) - c;
				hits[i].setNormal(normal.normalize());
				hits[i].setMaterialId( this->getMaterialId());
				rets[i] = true;
				continue;
			}
			rets[i] = false;
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

