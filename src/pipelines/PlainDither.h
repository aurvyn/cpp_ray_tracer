#pragma once

#include "PostProcessor.h"
#include "MotionBuffer.h"

class PlainDither : public Pipeline {
    public:
        PostProcessor* buildPipeline(
            Buffer<Vector3>* imageBuffer,
            Buffer<Vector3>* normalBuffer,
            Buffer<Vector3>* depthBuffer,
            MotionBuffer* motionBuffer,
            Buffer<Vector3>* positionBuffer,
            Camera camera
        ) override {
            // Effect *result = ConstMultiply((new BayerianDither( new RGBConvert(new ToneMapHSV(new HSVConvert(imageBuffer)))))->init(RGBPallete()), 255.0f);
            Effect *result = ConstMultiply((new FloydDither( new RGBConvert(new ToneMapHSV(new HSVConvert(imageBuffer)))))->setPallette(RGBPallete()), 255.0f);
            // Effect *result = ConstMultiply(new RGBConvert(new ToneMapHSV(new HSVConvert(imageBuffer))), 255.0f);

            return new PostProcessor(result);
        }
};
