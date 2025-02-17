#pragma once

#include "Buffer.h"
#include "Shader.h"

class MotionBuffer : public Buffer<Vector2>
{
public:
    using Buffer<Vector2>::Buffer;

    /**
     * Bidirectionally dilutes the motion buffer to eliminate blur cut-offs.
     * This function is mainly reserved for interal use by the MotionBlur effect.
     * 
     * Inspired by: https://casual-effects.com/research/McGuire2012Blur/McGuire12Blur.pdf
     * @param samples The number of samples to take for each pixel.
     * This controls how smoothly the dilution of the motion buffer is.
     * Higher number of samples means more smoothing.
     * @param chunkSize The size of the chunks to dilute, in pixels.
     * This should be scaled according to the movement of the entire scene.
     * If an object is too fast and the chunkSize is too small, the blur will appear to be cut out.
     * If the resolution is high, then this number might need to be increased.
     */
    MotionBuffer *dilate(int samples = 32, int chunkSize = 8) {
        int width = this->getWidth();
        int height = this->getHeight();
        Buffer<Vector2> neighborHood = getNeighborhood(chunkSize);
        MotionBuffer finalBuffer(width, height);
        for (int i = 0; i < neighborHood.getWidth(); ++i) {
            for (int j = 0; j < neighborHood.getHeight(); ++j) {
                int offset_x = i*chunkSize;
                int offset_y = j*chunkSize;
                int chunkWidth = std::min(chunkSize, width - offset_x);
                int chunkHeight = std::min(chunkSize, height - offset_y);
                Vector2 guidingVelocity = neighborHood.at(i, j);
                for (int m = 0; m < chunkWidth; ++m) {
                    for (int n = 0; n < chunkHeight; ++n) {
                        int x = offset_x+m;
                        int y = offset_y+n;
                        Vector2 velocity(0.0f);
                        for (int k = 1-samples/2; k < samples/2; ++k) {
                            Vector2 samplePos = Vector2(x, y) + guidingVelocity * k / samples;
                            int sx = clamp(0, width - 1, samplePos[0]); // sample x
                            int sy = clamp(0, height - 1, samplePos[1]); // sample y
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

private:
    Buffer<Vector2> getNeighborBuffer(int chunkSize) {
        int width = this->getWidth();
        int height = this->getHeight();
        Buffer<Vector2> neighborBuffer(width/chunkSize + 1, height/chunkSize + 1);
        for (int i = 0; i < neighborBuffer.getWidth(); ++i) {
            for (int j = 0; j < neighborBuffer.getHeight(); ++j) {
                int offset_x = i*chunkSize;
                int offset_y = j*chunkSize;
                int chunkWidth = std::min(chunkSize, width - offset_x);
                int chunkHeight = std::min(chunkSize, height - offset_y);
                float highestMagnitude = 0.0f;
                Vector2 guidingVelocity = this->at(offset_x, offset_y);
                for (int m = 0; m < chunkWidth; ++m) {
                    for (int n = 0; n < chunkHeight; ++n) {
                        Vector2 motion = this->at(offset_x+m, offset_y+n);
                        float magnitude = motion.length();
                        if (magnitude > highestMagnitude) {
                            highestMagnitude = magnitude;
                            guidingVelocity = motion;
                        }
                    }
                }
                neighborBuffer.at(i, j) = guidingVelocity;
            }
        }
        return neighborBuffer;
    }

    Buffer<Vector2> getNeighborhood(int chunkSize) {
        Buffer<Vector2> neighborBuffer = getNeighborBuffer(chunkSize);
        Buffer<Vector2> neighborHood(neighborBuffer.getWidth(), neighborBuffer.getHeight());
        for (int i = 0; i < neighborBuffer.getWidth(); ++i) {
            for (int j = 0; j < neighborBuffer.getHeight(); ++j) {
                Vector2 guidingVelocity = neighborBuffer.at(i, j);
                Vector2 neighborVelocity = Vector2(0.0f);
                float highestMagnitude = 0.0f;
                for (int k = -1; k <= 1; ++k) {
                    for (int l = -1; l <= 1; ++l) {
                        int sx = i + k; // sample x
                        int sy = j + l; // sample y
                        if (sx >= 0 && sx < neighborBuffer.getWidth()
                         && sy >= 0 && sy < neighborBuffer.getHeight()) {
                            float magnitude = neighborBuffer.at(sx, sy).length();
                            if (magnitude > highestMagnitude) {
                                highestMagnitude = magnitude;
                                neighborVelocity = neighborBuffer.at(sx, sy);
                            }
                        }
                    }
                }
                neighborHood.at(i, j) = neighborVelocity;
            }
        }
        return neighborHood;
    }
};
