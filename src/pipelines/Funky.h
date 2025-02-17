#pragma once

#include "PostProcessor.h"
#include "MotionBuffer.h"

class Funky : public Pipeline {
    public:
        PostProcessor* buildPipeline(
            Buffer<Vector3>* imageBuffer,
            Buffer<Vector3>* normalBuffer,
            Buffer<Vector3>* depthBuffer,
            MotionBuffer* motionBuffer
        ) override {
            // Buffer<Vector3> *imageBuffer2 = copyBuffer(imageBuffer);
            // Buffer<Vector3> *imageBuffer3 = copyBuffer(imageBuffer);
            float minDepth = INFINITY;
            
            for(int x = 0; x < depthBuffer->getWidth(); x++) {
                for(int y = 0; y < depthBuffer->getWidth(); y++) {
                    minDepth = std::min(depthBuffer->at(x,y)[0], minDepth);
                    
                }
            }
            // Buffer<Vector3> *sinOverlayBuf = new Buffer<Vector3>(imageBuffer->getWidth(), imageBuffer->getHeight());
            float scl = 360.0f / (1 - minDepth);
            printf("min/max = %f/%f\n", minDepth, 1);
            // Effect *depthHueOverlay = (new Bound((new Bound(ConstMultiply(ConstAdd(NoOp(depthBuffer), -minDepth), scl)))->init(1.0f, 0.0f, 1, 1.0f, 1.0f)))->init(1.0f, 1.0f, 2, 1.0f, 1.0f);
            Effect *depthHueOverlay = ConstSet(ConstMultiply(ConstAdd(NoOp(depthBuffer), -minDepth), scl), 2 | 4, Vector3(0.0f, 1.0f, 1.0f));
            Effect *base = new HSVConvert(new Vignette(imageBuffer));
            
            // Effect *result = ConstMultiply(new RGBConvert(new ToneMapHSV(Add(depthHueOverlay, base))), 255.0f);

            Effect *result = ConstMultiply((new FloydDither(new RGBConvert(new ToneMapHSV(base))))->init(true), 255.0f);
            // recommended threshold values: 
            //  bunny-scene: 0.995
            
            // Effect *redMaskBase = ColorMultiply(Threshold(NoOp(imageBuffer2), 0.995, AVG), Vector3(127.0f/255.0f, 0.0f, 0.0f));
            // Effect *redMask = Multiply(sinOverlay, redMaskBase);
            // Effect *base = new Vignette(imageBuffer);
            return new PostProcessor(result);
        }
};
