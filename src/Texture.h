#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture
{
public:
    Texture(char const * filename)
    {
        textureData = stbi_load(filename, &width, &height, &channels, 0);
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    Vector3 getPixel(const int x, const int y) const
    {
        const int index = (y * width + x) * channels;
        return Vector3(textureData[index], textureData[index + 1], textureData[index + 2]) / 255.0;
    }

    private:
        int width, height;
        unsigned char * data;
        int channels;

        unsigned char* textureData;
};
