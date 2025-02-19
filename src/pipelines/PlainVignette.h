#pragma once

#include "PostProcessor.h"
#include "MotionBuffer.h"

class PlainVignette : public Pipeline {
    public:
        PostProcessor* buildPipeline(
            Buffer<Vector3>* imageBuffer,
            Buffer<Vector3>* normalBuffer,
            Buffer<Vector3>* depthBuffer,
            MotionBuffer* motionBuffer
        ) override {
            // Effect *result = ConstMultiply((new Vignette( new RGBConvert(new ToneMapHSV(new HSVConvert(imageBuffer)))))->init(Vector4(0,0,0,0)), 255.0f);
            Effect *result = ConstMultiply((new Vignette( new RGBConvert(new ToneMapHSV(new HSVConvert(imageBuffer)))))->init(Vector4(0.3,0.1,0.1,0), 1.5f, 2.0f, 2.0f), 255.0f);
            // Effect *result = ConstMultiply(new RGBConvert(new ToneMapHSV(new HSVConvert(imageBuffer))), 255.0f);

            return new PostProcessor(result);
        }
};
