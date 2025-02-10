#pragma once

#include "Buffer.h"
#include "Shader.h"

class MotionBuffer : public Buffer<Vector2>
{
public:
    using Buffer<Vector2>::Buffer;

    /**
     * Bidirectionally dilates the motion buffer to eliminate blur cut-offs.
     * This function is mainly reserved for interal use by the MotionBlur effect.
     * @param samples The number of samples to take for each pixel.
     * This controls how smoothly the dilution of the motion buffer is.
     * Higher number of samples means more smoothing.
     * @param chunkSize The size of the chunks to dilate, in pixels.
     * This should be scaled according to the movement of the entire scene.
     * If an object is too fast and the chunkSize is too small, the blur will appear to be cut out.
     * If the resolution is high, then this number might need to be increased.
     */
    MotionBuffer *dilate(int samples = 32, int chunkSize = 32) {
        int width = this->getWidth();
        int height = this->getHeight();
        MotionBuffer finalBuffer(width, height);
        for (int i = 0; i <= width/chunkSize; ++i) {
            for (int j = 0; j <= height/chunkSize; ++j) {
                int offset_x = i*chunkSize;
                int offset_y = j*chunkSize;
                int chunkWidth = std::min(chunkSize, width - offset_x);
                int chunkHeight = std::min(chunkSize, height - offset_y);
                float highestMagnitude = 0.0f;
                Vector2 guidingVelocity = this->at(offset_x, offset_y);
                int m, n;
                for (m = 0; m < chunkWidth; ++m) {
                    for (n = 0; n < chunkHeight; ++n) {
                        Vector2 motion = this->at(offset_x+m, offset_y+n);
                        float magnitude = motion.length();
                        if (magnitude > highestMagnitude) {
                            highestMagnitude = magnitude;
                            guidingVelocity = motion;
                        }
                    }
                }
                for (m = 0; m < chunkWidth; ++m) {
                    for (n = 0; n < chunkHeight; ++n) {
                        int x = offset_x+m;
                        int y = offset_y+n;
                        Vector2 velocity(0.0f);
                        for (int k = 1-samples/2; k < samples/2; ++k) {
                            Vector2 samplePos = Vector2(x, y) + guidingVelocity * k / samples;
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
