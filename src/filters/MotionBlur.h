#pragma once

#include "Effect.h"

class MotionBlur : public Effect
{
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
        size_t width = this->imageBuffer->getWidth();
        size_t height = this->imageBuffer->getHeight();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vector2 velocity = this->motionBuffer.at(x, y);
                Color total(0, 0, 0);
                for (int i = 0; i < this->samples; ++i) {

                }
            }
        }
    }
};