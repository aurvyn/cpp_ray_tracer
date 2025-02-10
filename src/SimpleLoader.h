#ifndef __LOADER
#define __LOADER

#include "ObjLoader.h"
#include "Scene.h"
#include "BVHTree.h"

class SimpleLoader
{
public:
	template<bool useArray=false>
	Scene getScene(size_t vertexCount, size_t triangleCount, float const * vertexVals, int const * triangleIndices)
	{
		Scene scene;
		PrimitiveArray * primitiveArray = new PrimitiveArray();
		
		for(size_t t=0; t<triangleCount; t++)
		{
			Vector3 triVertex[3];
			int const * rawTriangle = &triangleIndices[ t*3 ];
			for(size_t v=0; v<3; v++)
			{
				size_t vertexIndex = rawTriangle[v]*3;
				float const * rawVertexVals = &vertexVals[vertexIndex];
				triVertex[v] = floatToGenVec(rawVertexVals);
			}
			
			Triangle * tri = new Triangle(triVertex[0], triVertex[1], triVertex[2]);
			tri->setMaterialId(0);
			primitiveArray->add(tri);
		}
		
		if(useArray)
			scene.setRootPrimitive(primitiveArray);
		else {
			BVHTree * tree = new BVHTree();
			tree->setContents(primitiveArray);
			scene.setRootPrimitive(tree);
		}

		scene.addDefaultMaterial();
		scene.addDefaultLight();
		scene.addDefaultCamera();
		
		return scene;
	}
	
private:

	Vector3 floatToGenVec(float const * vals)
	{
		Vector3 v;
		v[0] = vals[0];
		v[1] = vals[1];
		v[2] = vals[2];
		return v;
	}
};

#endif

