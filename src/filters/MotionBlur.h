#pragma once

#include "Effect.h"

class MotionBlur : public Effect
{
    using Effect::Effect; // uses super constructor
    
    MotionBuffer motionBuffer;
    int samples;
public:
    MotionBlur *init(MotionBuffer motionBuffer, int samples = 32) {
        motionBuffer.dilate();
        this->motionBuffer = motionBuffer;
        this->samples = samples;
        return this;
    }

    // Applies the motion blur effect bidirectionally to the image buffer.
    void _apply() override
    {
        unsigned int width = imageBuffer->getWidth();
        unsigned int height = imageBuffer->getHeight();
        Buffer<Vector3> finalBuffer(width, height);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vector2 velocity = this->motionBuffer.at(x, y);
                Vector3 color(0.0f);
                for (int i = 1-samples/2; i < samples/2; ++i) {
                    Vector2 samplePos = Vector2(x, y) + velocity * i / samples;
                    unsigned int sx = clamp(0, width - 1, samplePos[0]);
                    unsigned int sy = clamp(0, height - 1, samplePos[1]);
                    color += imageBuffer->at(sx, sy);
                }
                finalBuffer.at(x, y) = color / samples;
            }
        }
        *imageBuffer = finalBuffer;
    }
};
