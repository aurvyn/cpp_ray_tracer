#pragma once

#include <iostream>
#include <fstream>

char *get_number(char *token, int pos) {
    char *ptr = &token[pos++];
    for (; pos < 16; ++pos) {
        if (token[pos] == '\"') {
            token[pos] = '\0';
            break;
        }
    }
    return ptr;
}

int parse_ishihara_svg_scene(obj_scene_data *data_out, char const *file_name)
{
    std::ifstream file(file_name);
	if (!file)
	{
		std::cerr << "Error reading file: " << file_name << std::endl;
		return 0;
	}
    std::ofstream mtl_file("../../tests/ishihara.mtl");
    std::ofstream obj_file("../../tests/ishihara.obj");
    mtl_file << "newmtl background\nKa 1 1 1\n" << std::endl;
    obj_file << "mtllib ishihara.mtl" << std::endl;
    char token[32], fill[32], *fill_ptr;
    int width, height, centerX, centerY;
	while (file >> token)
    {
        if (strequal(token, "<svg")) {
            file >> token;
            width = atoi(get_number(token, 7));
            centerX = width / 2;
            file >> token;
            height = atoi(get_number(token, 8));
            centerY = height / 2;
        }
        else if (strequal(token, "<circle")) {
            obj_file << "v ";
            char *ptr;
            for (int i = 0; i < 2; ++i) {
                file >> token;
                ptr = get_number(token, 4);
                obj_file << ptr << " ";
            }
            file >> token;
            ptr = get_number(token, 3);
            obj_file << "0\nvn 0 " << ptr << " 0\nvn " << ptr << " 0 0\nsp -1 -1 -2" << std::endl;
        }
        else if (strequal(token, "<g")) {
            file >> fill;
            fill_ptr = &fill[7];
            fill[13] = '\0';
            int r, g, b;
            sscanf(fill_ptr, "%2x%2x%2x", &r, &g, &b);
            obj_file << "\nusemtl " << fill_ptr << "\n" << std::endl;
            mtl_file << "newmtl " << fill_ptr << "\nKa " << (float)r/255 << " " << (float)g/255 << " " << (float)b/255 << "\n" << std::endl;
        }
    }
    // background
    obj_file << "\nv 0 0 0\nv " << width << " 0 0\nv 0 " << height << " 0\nv " << width << " " << height << " 0\nusemtl background\nf -4 -3 -2\nf -3 -2 -1" << std::endl;
    // camera
    obj_file << "\nv " << centerX << " " << centerY << " " << -centerY <<  "\nv " << centerX << " " << centerY << " 0\nvn 0 -1 0\ng Camera\nc -2 -1 -1" << std::endl;
    file.close();
    mtl_file.close();
    obj_file.close();
    parse_obj_scene(data_out, "../../tests/ishihara.obj");
    return 1;
}
