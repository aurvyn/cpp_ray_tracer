#ifndef __SHADER
#define __SHADER

#include <random>

#define clamp(a,b,v) (v) < (a) ? (a) : ( (v) > (b) ? (b) : (v) )
#define RAY_JITTER_EPSILON 0.0001f
#define MAX_RECURS 100

class HitDetails
{
private:
	Ray ray;
	Hitpoint hit;
	
	Vector3 hitPos;
	Vector3 n;
	Vector3 d;
	Vector3 v;
	Vector3 r;

public:
	
	HitDetails()
	{ }
	
	HitDetails(Ray const & ray, Hitpoint const & hit)
	{
		this->ray = ray;
		this->hit = hit;
		
		hitPos = ray.pointAtParameter(hit.getParameter());
		n = hit.getNormal().normalize();
		d = ray.getDirection();
		v = -d;
		r = d.reflect(n);
	}
	
	Vector3 position() const
	{ return hitPos; }
	
	Vector3 normal() const
	{ return n; }
	
	Vector3 direction() const
	{ return d; }
	
	Vector3 reflection() const
	{ return r; }
	
	Vector3 view() const
	{ return v; }
	
	size_t materialId() const
	{ return hit.getMaterialId(); }
};

class Shader
{
public:

	static float RandomFloat(float min, float max){
		float r = (float) rand() / (float) RAND_MAX;
		return min + r * (max - min);
	}

	static Ray getRandomReflection(Hitpoint const & hit, Vector3 const & loc){
		Vector3 normal = hit.getNormal();
		Vector3 randDir = Vector3(RandomFloat(-1,1), RandomFloat(-1,1), RandomFloat(-1,1));
		randDir.normalize();
		if (randDir.dot(normal) < 0) randDir *= -1;
		Ray r;
		r.setOrigin(loc);
		r.setDirection(randDir);
		return r;
	}

	static Vector3 shade(Ray const & ray, Hitpoint const & hit, Scene const & scene, size_t recurs=0)
	{
		Vector3 color;
		HitDetails hd = HitDetails(ray, hit);

		if(recurs > MAX_RECURS)
			return color;

		//Day 5
		/*
		Vector3 v = hd.normal();
		for(int i=0; i<3; i++)
			//v[i] = abs(v[i]*255);
			v[i] = v[i]*255.0f;
		return v;
		*/

		for(int l=0; l<scene.getLights().size(); l++)
		{
			Light* light = scene.getLights().at(l);
			color += Shader::computeLighting(hd, scene, *light);
		}
		
		Material const & surfaceMat = scene.getMaterials().at( hd.materialId() );
		float reflectCoef = surfaceMat.getReflectance();
		bool hasMirrorReflection = reflectCoef > 0.0f;
		
		//Day 6
		/*
		Vector3 Ia = surfaceMat.getKa();
		return Ia;
		*/


		//Day 7
		//return color;
		
		//Day 8
		//hasMirrorReflection = false;

		if(hasMirrorReflection)
		{
			Ray reflectRay;
			Hitpoint reflectHit;
			Vector3 reflectColor;
			reflectRay.setOrigin(hd.position() + hd.normal()*RAY_JITTER_EPSILON);
			reflectRay.setDirection( hd.reflection() );
			
			bool hitSomething = false;
			hitSomething = scene.getRootPrimitive()->intersect(reflectRay, reflectHit);
			if(hitSomething)
				reflectColor = Shader::shade(reflectRay, reflectHit, scene, recurs+1);
			else
				reflectColor = Vector3(0.0f);
			
			color = reflectColor * reflectCoef + color * (1.0f-reflectCoef);
		}
		
		return color;
	}
	
private:
	static Vector3 computeLighting(HitDetails const & hd,
						 Scene const & scene, Light const & light)
	{
		Material const & surfaceMat = scene.getMaterials().at( hd.materialId() );
		Material const & lightMat = scene.getMaterials().at( light.getMaterialId() );
		
		Vector3 l = (light.getPosition()-hd.position()).normalize();
		Vector3 lr = l.reflect(hd.normal());
		float lightDis = (light.getPosition()-hd.position()).length();
		float p = surfaceMat.getShininess();
		
		Ray shadowRay;
		Hitpoint shadowHit;
		shadowRay.setOrigin(hd.position() + hd.normal()*RAY_JITTER_EPSILON);
		shadowRay.setDirection(l);
		bool shadowHitSomething = scene.getRootPrimitive()->intersect(shadowRay, shadowHit);
		
		Vector3 Ia = surfaceMat.getKa() * lightMat.getKa();
		
		bool inShadow = shadowHit.getParameter() < lightDis;

		//Day 7
		//inShadow = false;

		if(inShadow)
			return Ia;
		
		float dCoef = (l.dot(hd.normal()));
		float sCoef = pow((hd.view().dot(lr)),p);
		dCoef = clamp(0.0f, 1.0f, dCoef);
		sCoef = clamp(0.0f, 1.0f, sCoef);
		
		Vector3 Id = surfaceMat.getKd() * dCoef * lightMat.getKd();
		Vector3 Is = surfaceMat.getKs() * sCoef * lightMat.getKs();
		
		Vector3 floatColor = (Ia + Id + Is);
		return floatColor;
	}
};

#endif

