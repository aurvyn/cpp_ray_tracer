
#define OBJ_LOADER_HEADER_LIB
#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "obj_parser.h"
#include "ishihara_svg_parser.h"

class objLoader
{
public:
	objLoader() {}
	~objLoader()
	{
		delete_obj_data(&data);
	}

	int load(char const * filename);
	int fromString(char const * objString, char const * mtlString);

	obj_vector **vertexList;
	obj_vector **normalList;
	obj_vector **textureList;
	
	obj_face **faceList;
	obj_sphere **sphereList;
	obj_plane **planeList;
	
	obj_light_point **lightPointList;
	obj_light_quad **lightQuadList;
	obj_light_disc **lightDiscList;
	
	obj_material **materialList;
	
	int vertexCount;
	int normalCount;
	int textureCount;

	int faceCount;
	int sphereCount;
	int planeCount;

	int lightPointCount;
	int lightQuadCount;
	int lightDiscCount;

	int materialCount;

	obj_camera *camera;
private:
	obj_scene_data data;
	void assign();
};

#endif
#ifndef OBJ_LOADER_HEADER_LIB
#include "objLoader.h"
#include "obj_parser.h"
#endif

int objLoader::load(char const * filename)
{
	int no_error = 1;
	if (strequal(&filename[strlen(filename) - 4], ".svg")) {
		no_error = parse_ishihara_svg_scene(&data, filename);
	} else {
		no_error = parse_obj_scene(&data, filename);
	}
	if(no_error)
		assign();
	return no_error;
}

int objLoader::fromString(char const * objString, char const * mtlString)
{
	int no_error = 1;
	no_error = parse_obj_scene_string(&data, objString, mtlString);
	if(no_error)
		assign();
	return no_error;
}


void objLoader::assign()
{
	this->vertexCount = data.vertex_count;
	this->normalCount = data.vertex_normal_count;
	this->textureCount = data.vertex_texture_count;

	this->faceCount = data.face_count;
	this->sphereCount = data.sphere_count;
	this->planeCount = data.plane_count;

	this->lightPointCount = data.light_point_count;
	this->lightDiscCount = data.light_disc_count;
	this->lightQuadCount = data.light_quad_count;

	this->materialCount = data.material_count;

	this->vertexList = data.vertex_list;
	this->normalList = data.vertex_normal_list;
	this->textureList = data.vertex_texture_list;

	this->faceList = data.face_list;
	this->sphereList = data.sphere_list;
	this->planeList = data.plane_list;

	this->lightPointList = data.light_point_list;
	this->lightDiscList = data.light_disc_list;
	this->lightQuadList = data.light_quad_list;

	this->materialList = data.material_list;

	this->camera = data.camera;
}

