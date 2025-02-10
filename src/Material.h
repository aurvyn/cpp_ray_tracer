#ifndef __MATERIAL
#define __MATERIAL

#include "GenVector.h"
#include "Texture.h"

class Material
{
public:
	Material()
	{
		ka = Vector3(0.2);
		kd = Vector3(0.5);
		kd = Vector3(0.5);
		reflectance = 0.0;
		translucency = 0.0;
		texture = nullptr;
	}

	void setKa(Vector3 c) { this->ka = c; }
	void setKd(Vector3 c) { this->kd = c; }
	void setKs(Vector3 c) { this->ks = c; }
	void setReflectance(float reflectance) { this->reflectance = reflectance; }
	void setTranslucency(float translucency) { this->translucency = translucency; }
	void setShininess(float shininess) { this->shininess = shininess; }
	void loadTexture(char const * filename) { this->texture = new Texture(filename); }
	
	Vector3 getKa() const { return this->ka; }
	Vector3 getKd() const { return this->kd; }
	Vector3 getKd(const float u, const float v) const
	{
		return ( texture != nullptr ) ? this->texture->getUV(u, v) : this->kd;
	}
	Vector3 getKd(const Vector2& uv) const { return this->texture->getUV(uv.c[0], uv.c[1]); }
	Vector3 getKs() const { return this->ks; }
	float getReflectance() const { return this->reflectance; }
	float getTranslucency() const { return this->translucency; }
	float getShininess() const { return this->shininess; }
				
	
private:
	Vector3 ka, kd, ks;
	float reflectance;
	float translucency;
	float shininess;

	Texture* texture;
};

#endif

