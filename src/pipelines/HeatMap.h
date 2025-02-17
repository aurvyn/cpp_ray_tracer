#pragma once

#include "PostProcessor.h"
#include "MotionBuffer.h"

class HeatMap : public Pipeline {
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
            Effect *depthHueOverlay = ColorAdd(ConstMultiply(ColorAdd(NoOp(depthBuffer), Vector3(-minDepth, 0.0f, 0.0f)), scl), Vector3(0.0f, 1.0f, 1.0f));
            // Effect *sinOverlay = ColorMultiply(ConstAdd(VSin(sinOverlayBuf), 1.0f), Vector3(180.0f, 0.0f, 0.0f));
            Effect *base = new HSVConvert(new Vignette(imageBuffer));
            
            // Effect *result = ConstMultiply(new RGBConvert(new ToneMapHSV(Add(depthHueOverlay, base))), 255.0f);

            Effect *result = ConstMultiply(new RGBConvert(new ToneMapHSV(depthHueOverlay)), 255.0f);
            // recommended threshold values: 
            //  bunny-scene: 0.995
            
            // Effect *redMaskBase = ColorMultiply(Threshold(NoOp(imageBuffer2), 0.995, AVG), Vector3(127.0f/255.0f, 0.0f, 0.0f));
            // Effect *redMask = Multiply(sinOverlay, redMaskBase);
            // Effect *base = new Vignette(imageBuffer);
            return new PostProcessor(result);
        }
};
