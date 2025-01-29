#ifndef __SCENE
#define __SCENE

#include <vector>
#include "Camera.h"
#include "Primitive.h"
#include "PrimitiveArray.h"
#include "Material.h"
#include "Light.h"

class Scene
{
public:
	Scene()
	{
		rootPrimitive = NULL;
	}
	
	void setRootPrimitive(Primitive * primitive)
	{ this->rootPrimitive = primitive; }
	
	void setMaterials(std::vector<Material> materials)
	{ this->materials = materials; }
	
	void setCamera(Camera camera)
	{ this->camera = camera; }
	
	void setLights(std::vector<Light*> lights)
	{ this->lights = lights; }

	Primitive * getRootPrimitive() const
	{ return this->rootPrimitive; }
	
	std::vector<Material> const & getMaterials() const
	{ return this->materials; }
	
	Camera getCamera() const
	{ return this->camera; }
	
	std::vector<Light*> const & getLights() const
	{ return this->lights; }
	
	void addDefaultLight()
	{
		Vector3 pos;
		if(this->rootPrimitive != NULL)
			pos = this->rootPrimitive->getBBMax();
		
		Light * n = new Light();
		n->setPosition(pos);
		
		Material mat;
		materials.push_back(mat);
		
		n->setMaterialId(materials.size() - 1);
		lights.push_back(n);
	}
	
	void addDefaultCamera()
	{
		Vector3 pos = Vector3(1,1,1);
		Vector3 lookAt;
		Vector3 up = Vector3(0,1,0);
		
		if(this->rootPrimitive != NULL)
		{
			pos = this->rootPrimitive->getBBMax();
			lookAt = this->rootPrimitive->getCenter();
		}
		camera = Camera(pos, lookAt, up);
	}
	
	void addDefaultMaterial()
	{
		Material mat;
		materials.push_back(mat);
	}
	
private:
	Primitive * rootPrimitive;
	Camera camera;
	std::vector<Material> materials;
	std::vector<Light*> lights;
};

#endif

