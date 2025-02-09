#pragma once

#include "Effect.h"

class MotionBlur : public Effect
{
    using Effect::Effect; // uses super constructor
    
    Buffer<Vector2> motionBuffer;
    unsigned int samples;
public:
    MotionBlur *init(Buffer<Vector2> motionBuffer, unsigned int samples = 10) {
        this->motionBuffer = motionBuffer;
        this->samples = samples;
        return this;
    }

    void _apply() override
    {
        size_t width = imageBuffer->getWidth();
        size_t height = imageBuffer->getHeight();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vector2 velocity = this->motionBuffer.at(x, y);
                Vector3 color = imageBuffer->at(x, y);
                for (int i = 1; i < samples; ++i) {
                    Vector2 samplePos = Vector2(x, y) + velocity * i;
                    color += imageBuffer->at(samplePos[0], samplePos[1]);
                }
                imageBuffer->at(x, y) = color / samples;
            }
        }
    }
};