#ifndef __TRIANGLE
#define __TRIANGLE

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

		this->bary = new Barycentric(vertexA, vertexB, vertexC);
	}

	virtual bool intersect(Ray const & ray, Hitpoint & hit) const
	{
		Vector3 pos = ray.getOrigin();
		Vector3 dir = ray.getDirection();
		Vector3 v0 = this->vertex[0];
		Vector3 v1 = this->vertex[1];
		Vector3 v2 = this->vertex[2];
		Vector3 n = this->normal;
		
		double nDotDir = n.dot(dir);
		if (nDotDir == 0)
			return false;

		double distance = -(n.dot(pos) + -n.dot(v0)) / nDotDir;
		if (distance < 0)
			return false;

		Vector3 location = pos + dir * distance;

		Vector3 c;

		Vector3 bLoc = location - v1;
		c = (v2 - v1).cross(bLoc);
		double u = n.dot(c);
		if (u < 0)
			return false;

		Vector3 cLoc = location - v2;
		c = (v0 - v2).cross(cLoc);
		double v = n.dot(c);
		if (v < 0)
			return false;

		Vector3 aLoc = location - v0;
		c = (v1 - v0).cross(aLoc);
		double w = n.dot(c);
		if (w < 0)
			return false;
				
		hit.setParameter(distance);
		hit.setSurfaceCoords(Vector2(u, v));
		hit.setNormal(this->normal);
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
	size_t materialId;

	Barycentric* bary;
};

#endif

