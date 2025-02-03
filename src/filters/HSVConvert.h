#pragma once

#include "Effect.h"

class HSVConvert : public Effect
{
    public:
        void applyEffect(Buffer<Vector3> & floatBuffer) override
        {
            size_t resX = floatBuffer.getWidth();
            size_t resY = floatBuffer.getHeight();

            for(int y=0; y<resY; y++) {
                for(int x=0; x<resX; x++) {
                    Vector3 rgbColor = floatBuffer.at(x,y);
                    Vector3 hsvColor = rgbToHsv(rgbColor);
                    floatBuffer.at(x,y) = hsvColor;
                }
            }
        }
};

