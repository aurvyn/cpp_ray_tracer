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
		setCenter();
	}
	
	virtual bool intersect(Ray const & ray, Hitpoint & hit) const
	{
		return MöllerTrumboreIntersection(ray, hit);
	}

	virtual std::array<bool, N*N> packetIntersect(RayPacket const & rays, Hitpoint* hits) const
	{	
		return packetMöllerTrumboreIntersection(rays, hits);
	}

	std::array<bool, N*N> packetMöllerTrumboreIntersection(RayPacket const & rays, Hitpoint* hits) const
	{
		std::array<bool, N*N> rets;
		for (int i = 0; i < N*N; i++){
			bool intersect = true;
			Vector3 e1 = vertex[1] - vertex[0];
			Vector3 e2 = vertex[2] - vertex[0];
			Vector3 cross = rays.getDirections()[i].cross(e2);
			float det = e1.dot(cross);

			if (det > -EPSILON && det < EPSILON){
				rets[i] = false;
				continue;
			}

			float invDet = 1.0 / det;
			Vector3 s = rays.getOrigin() - vertex[0];
			float u = invDet * s.dot(cross);

			if ((u < 0 && abs(u) > EPSILON) || (u > 1 && abs(u-1) > EPSILON)){
				rets[i] = false;
				continue;
			}

			Vector3 sCross = s.cross(e1);
			float v = invDet * rays.getDirections()[i].dot(sCross);

			if ((v < 0 && abs(v) > EPSILON) || (u + v > 1 && abs(u + v - 1) > EPSILON)){
				rets[i] = false;
				continue;
			}

			float t = invDet * e2.dot(sCross);

			if (t > EPSILON)
			{
				bool isCloser = t < hits[i].getParameter();
				if(!isCloser){
					rets[i] = false;
					continue;
				}
				
					
				hits[i].setParameter(t);
				hits[i].setNormal(this->normal);
				hits[i].setMaterialId( this->getMaterialId());
				rets[i] = intersect;
			}else{
				rets[i] = false;
			}
		}
		return rets;
	}

	bool MöllerTrumboreIntersection(Ray const & ray, Hitpoint & hit) const
	{
		Vector3 e1 = vertex[1] - vertex[0];
		Vector3 e2 = vertex[2] - vertex[0];
		Vector3 cross = ray.getDirection().cross(e2);
		float det = e1.dot(cross);

		if (det > -EPSILON && det < EPSILON){
			return false;
		}

		float invDet = 1.0 / det;
		Vector3 s = ray.getOrigin() - vertex[0];
		float u = invDet * s.dot(cross);

		if ((u < 0 && abs(u) > EPSILON) || (u > 1 && abs(u-1) > EPSILON)){
			return false;
		}

		Vector3 sCross = s.cross(e1);
		float v = invDet * ray.getDirection().dot(sCross);

		if ((v < 0 && abs(v) > EPSILON) || (u + v > 1 && abs(u + v - 1) > EPSILON)){
        	return false;
		}

		float t = invDet * e2.dot(sCross);

		if (t > EPSILON)
		{
			bool isCloser = t < hit.getParameter();
			if(!isCloser)
				return false;
				
			hit.setParameter(t);
			hit.setNormal(this->normal);
			hit.setMaterialId( this->getMaterialId());
			return true;
		}else{
			return false;
		}
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
};

#endif

