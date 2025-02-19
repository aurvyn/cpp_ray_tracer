#ifndef __OBJ_LOADER
#define __OBJ_LOADER

#include <iostream>
#include "obj_loader/objLoader.h"
#include "Scene.h"
#include "BVHTree.h"

class ObjLoader
{
public:
	template<bool useArray=false>
	Scene getScene(char const * filename)
	{
		Scene scene;
		
		objLoader objData = objLoader();
		
		//if no args, try test.obj
		objData.load((char*)filename);
		
		PrimitiveArray * primitiveArray = new PrimitiveArray();
		
		for(int i=0; i<objData.sphereCount; i++)
		{
			Vector3 pos, up, side, motion;
			obj_sphere *o = objData.sphereList[i];
			pos = objToGenVec(objData.vertexList[ o->pos_index ]);
			up = objToGenVec(objData.normalList[ o->up_normal_index ]);
			side = objToGenVec(objData.normalList[ o->equator_normal_index ]);
			if (strequal(objData.materialList[o->material_index]->name, "red")) {
				motion = Vector3(1, 0, 0);
			} else if (strequal(objData.materialList[o->material_index]->name, "green")) {
				motion = Vector3(0, 1, 0);
			} else if (strequal(objData.materialList[o->material_index]->name, "blue")) {
				motion = Vector3(0, 0, 1);
			}
			
			Sphere * sphere = new Sphere(pos, up.length(), motion);
			sphere->setMaterialId(o->material_index);
			primitiveArray->add(sphere);
		}
		
		for(int i=0; i<objData.faceCount; i++)
		{
			Vector3 a, b, c, motion;
			obj_face *o = objData.faceList[i];
			a = objToGenVec(objData.vertexList[ o->vertex_index[0] ]);
			b = objToGenVec(objData.vertexList[ o->vertex_index[1] ]);
			c = objToGenVec(objData.vertexList[ o->vertex_index[2] ]);
			if (strequal(objData.materialList[o->material_index]->name, "short_box")) {
				motion = Vector3(100, 0, 0);
			} else if (strequal(objData.materialList[o->material_index]->name, "grey")) {
				motion = Vector3(0, 0, .05);
			}
			
			Triangle * tri = new Triangle(a, b, c, motion);
			tri->setMaterialId(o->material_index);
			primitiveArray->add(tri);
		}
		
		if(useArray)
			scene.setRootPrimitive(primitiveArray);
		else {
			BVHTree * tree = new BVHTree();
			tree->setContents(primitiveArray);
			scene.setRootPrimitive(tree);
		}
		
		std::vector<Material> materials;
		for(int i=0; i<objData.materialCount; i++)
		{
			obj_material *mtl = objData.materialList[i];
			Material mat;
			//printf(" name: %s", mtl->name);
			
			Vector3 ka = Vector3(mtl->amb[0], mtl->amb[1], mtl->amb[2]);
			Vector3 kd = Vector3(mtl->diff[0], mtl->diff[1], mtl->diff[2]);
			Vector3 ks = Vector3(mtl->spec[0], mtl->spec[1], mtl->spec[2]);
			
			float reflect = mtl->reflect;
			float trans = mtl->trans;
			float shiny = mtl->shiny;
			
			mat.setKa(ka);
			mat.setKd(kd);
			mat.setKs(ks);
			mat.setReflectance(reflect);
			mat.setTranslucency(trans);
			mat.setShininess(shiny);
			
			//printf(" glossy: %i\n", mtl->glossy);
			//printf(" refact: %.2f\n", mtl->refract_index);
			
			//printf(" texture: %s\n", mtl->texture_filename);
			
			materials.push_back(mat);
		}
		scene.setMaterials(materials);
		
		if(objData.lightPointCount > 0)
		{
			std::vector<Light*> lights;
			for(int i=0; i<objData.lightPointCount; i++)
			{
				Vector3 pos;
				obj_light_point *o = objData.lightPointList[i];
				pos = objToGenVec(objData.vertexList[ o->pos_index ]);
				
				Light * n = new Light();
				n->setPosition(pos);
				n->setMaterialId(o->material_index);
				lights.push_back(n);
			}
			scene.setLights(lights);
		}
		else
		{
			scene.addDefaultLight();
		}
		
		Camera camera;
		if(objData.camera != NULL)
		{
			Vector3 pos = objToGenVec( objData.vertexList[ objData.camera->camera_pos_index ] );
			Vector3 lookAt = objToGenVec( objData.vertexList[ objData.camera->camera_look_point_index ] );
			Vector3 up = objToGenVec( objData.normalList[ objData.camera->camera_up_norm_index ] );
			camera = Camera(pos, lookAt, up, Vector3(0, 0, 0));
			scene.setCamera(camera);
		}
		else
		{
			scene.addDefaultCamera();
		}
		
		return scene;
	}
	
private:
	void printVec(Vector3 const & v)
	{
		printf("%f %f %f\n", v[0], v[1], v[2]);
	}
	
	Vector3 objToGenVec(obj_vector const * objVec)
	{
		Vector3 v;
		v[0] = objVec->e[0];
		v[1] = objVec->e[1];
		v[2] = objVec->e[2];
		return v;
	}
};

#endif

