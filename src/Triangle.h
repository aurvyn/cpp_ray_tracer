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

	virtual std::array<bool, N*M> packetIntersect(RayPacket const & rays, Hitpoint* hits) const
	{	
		return packetMöllerTrumboreIntersection(rays, hits);
	}

	std::array<bool, N*M> packetMöllerTrumboreIntersection(RayPacket const & rays, Hitpoint* hits) const
	{
		std::array<bool, N*M> rets;
		Vector3 e1 = vertex[1] - vertex[0];
		Vector3 e2 = vertex[2] - vertex[0];
		__m256 e1_x = _mm256_set1_ps(e1[0]);
		__m256 e1_y = _mm256_set1_ps(e1[1]);
		__m256 e1_z = _mm256_set1_ps(e1[2]);
		__m256 e2_x = _mm256_set1_ps(e2[0]);
		__m256 e2_y = _mm256_set1_ps(e2[1]);
		__m256 e2_z = _mm256_set1_ps(e2[2]);

		__m256 v0_x = _mm256_set1_ps(vertex[0][0]);
		__m256 v0_y = _mm256_set1_ps(vertex[0][1]);
		__m256 v0_z = _mm256_set1_ps(vertex[0][2]);

		__m256 dir_x = _mm256_loadu_ps(rays.getDirections()[0].c);
		__m256 dir_y = _mm256_loadu_ps(rays.getDirections()[1].c);
		__m256 dir_z = _mm256_loadu_ps(rays.getDirections()[2].c);

		__m256 orig_x = _mm256_set1_ps(rays.getOrigin()[0]);
		__m256 orig_y = _mm256_set1_ps(rays.getOrigin()[1]);	
		__m256 orig_z = _mm256_set1_ps(rays.getOrigin()[2]);
		
		__m256 cross_x = _mm256_sub_ps(_mm256_mul_ps(dir_y, e2_z), _mm256_mul_ps(dir_z, e2_y));
		__m256 cross_y = _mm256_sub_ps(_mm256_mul_ps(dir_z, e2_x), _mm256_mul_ps(dir_x, e2_z));
		__m256 cross_z = _mm256_sub_ps(_mm256_mul_ps(dir_x, e2_y), _mm256_mul_ps(dir_y, e2_x));

		__m256 det = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(e1_x, cross_x), _mm256_mul_ps(e1_y, cross_y)), _mm256_mul_ps(e1_z, cross_z));

		__m256 eps      = _mm256_set1_ps(EPSILON);
        __m256 neg_eps  = _mm256_set1_ps(-EPSILON);

		__m256 mask_det = _mm256_or_ps(_mm256_cmp_ps(det, neg_eps, _CMP_LT_OQ), _mm256_cmp_ps(det, eps, _CMP_GT_OQ));

		__m256 invDet = _mm256_div_ps(_mm256_set1_ps(1.0f), det);

		__m256 s_x = _mm256_sub_ps(orig_x, v0_x);
		__m256 s_y = _mm256_sub_ps(orig_y, v0_y);
		__m256 s_z = _mm256_sub_ps(orig_z, v0_z);

		__m256 dot_s_cross = _mm256_add_ps( _mm256_add_ps(_mm256_mul_ps(s_x, cross_x), _mm256_mul_ps(s_y, cross_y)), _mm256_mul_ps(s_z, cross_z));
		__m256 u = _mm256_mul_ps(invDet, dot_s_cross);

		__m256 mask_u = _mm256_and_ps(_mm256_cmp_ps(u, _mm256_set1_ps(0.0f),  _CMP_GE_OQ),
                                   _mm256_cmp_ps(u, _mm256_set1_ps(1.0f),  _CMP_LE_OQ));
								   
        __m256 mask_valid = _mm256_and_ps(mask_det, mask_u);

        __m256 sCross_x = _mm256_sub_ps(_mm256_mul_ps(s_y, e1_z), _mm256_mul_ps(s_z, e1_y));
        __m256 sCross_y = _mm256_sub_ps(_mm256_mul_ps(s_z, e1_x), _mm256_mul_ps(s_x, e1_z));
        __m256 sCross_z = _mm256_sub_ps(_mm256_mul_ps(s_x, e1_y), _mm256_mul_ps(s_y, e1_x));

        __m256 dot_dir_sCross = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(dir_x, sCross_x), _mm256_mul_ps(dir_y, sCross_y)), _mm256_mul_ps(dir_z, sCross_z));
        __m256 v = _mm256_mul_ps(invDet, dot_dir_sCross);

        __m256 mask_v = _mm256_and_ps(_mm256_cmp_ps(v, _mm256_set1_ps(0.0f),  _CMP_GE_OQ), _mm256_cmp_ps(_mm256_add_ps(u, v), _mm256_set1_ps(1.0f),  _CMP_LE_OQ));
        mask_valid = _mm256_and_ps(mask_valid, mask_v);

        __m256 dot_e2_sCross = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(e2_x, sCross_x), _mm256_mul_ps(e2_y, sCross_y)), _mm256_mul_ps(e2_z, sCross_z));
        __m256 t = _mm256_mul_ps(invDet, dot_e2_sCross);

        __m256 mask_t = _mm256_cmp_ps(t, eps,  _CMP_GT_OQ);
        mask_valid = _mm256_and_ps(mask_valid, mask_t);

        alignas(32) float t_array[8];
		alignas(32) int mask_array[8];
		_mm256_storeu_ps(t_array, t);
		_mm256_store_si256(reinterpret_cast<__m256i*>(mask_array), _mm256_castps_si256(mask_valid));

        for (int i = 0; i < 8; i++) {
            if (mask_array[i] != 0.0f) {
                if (t_array[i] < hits[i].getParameter()) {
                    hits[i].setParameter(t_array[i]);
                    hits[i].setNormal(this->normal);
                    hits[i].setMaterialId(this->getMaterialId());
                    rets[i] = true;
                }
                else {
                    rets[i] = false;
                }
            }
            else {
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

