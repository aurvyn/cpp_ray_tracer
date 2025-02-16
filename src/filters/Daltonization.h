#pragma once

#include "Effect.h"

class Daltonization : public Effect
{
    using Effect::Effect; // uses super constructor
    
public:
    // References:
    // - https://research.nvidia.com/publication/2023-05_daltonization
    // - https://en.wikipedia.org/wiki/YCbCr#ITU-R_BT.709_conversion
    void _apply() override
    {
        unsigned int width = imageBuffer->getWidth();
        unsigned int height = imageBuffer->getHeight();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vector3 rgb = imageBuffer->at(x, y);
                Vector3 ycc(
                    .2126*rgb[0] + .7152*rgb[1] + .0722*rgb[2],
                    -.1146*rgb[0] - .3854*rgb[1] + .5*rgb[2],
                    .5*rgb[0] - 0.4542*rgb[1] - .0458*rgb[2]
                );
                // TODO: Transformation to the Dichromat's line of visibility
                // TODO: Back-Projection
                imageBuffer->at(x, y) = Vector3(
                    ycc[0] + 1.5748*ycc[2],
                    ycc[0] - .1873*ycc[1] - .4681*ycc[2],
                    ycc[0] + 1.8556*ycc[1]
                );
            }
        }
    }
};
