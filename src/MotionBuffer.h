#pragma once

#include "Buffer.h"
#include "Shader.h"

class MotionBuffer : public Buffer<Vector2>
{
public:
    using Buffer<Vector2>::Buffer;

    MotionBuffer *dilute(int samples = 10, int chunkSize = 8) {
        unsigned int width = this->getWidth();
        unsigned int height = this->getHeight();
        MotionBuffer finalBuffer(width, height);
        for (int i = 0; i < width/chunkSize; ++i) {
            for (int j = 0; j < height/chunkSize; ++j) {
                int offset_x = i*chunkSize;
                int offset_y = j*chunkSize;
                float highestMagnitude = 0.0f;
                Vector2 guidingVelocity = this->at(offset_x, offset_y);
                int m, n;
                for (m = 0; m < chunkSize; ++m) {
                    for (n = 0; n < chunkSize; ++n) {
                        Vector2 motion = this->at(offset_x+m, offset_y+n);
                        float magnitude = motion.length();
                        if (magnitude > highestMagnitude) {
                            highestMagnitude = magnitude;
                            guidingVelocity = motion;
                        }
                    }
                }
                for (m = 0; m < chunkSize; ++m) {
                    for (n = 0; n < chunkSize; ++n) {
                        int x = offset_x+m;
                        int y = offset_y+n;
                        Vector2 velocity(0.0f);
                        for (int k = 1-samples/2; k < samples/2; ++k) {
                            Vector2 samplePos = Vector2(x, y) + guidingVelocity * k;
                            unsigned int sx = clamp(0, width - 1, samplePos[0]);
                            unsigned int sy = clamp(0, height - 1, samplePos[1]);
                            velocity += this->at(sx, sy);
                        }
                        finalBuffer.at(x, y) = velocity / samples;
                    }
                }
            }
        }
        *this = finalBuffer;
        return this;
    }
};
