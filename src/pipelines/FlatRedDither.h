#pragma once

#include "PostProcessor.h"
#include "MotionBuffer.h"

class FlatRedDither : public Pipeline {
    public:
        PostProcessor* buildPipeline(
            Buffer<Vector3>* imageBuffer,
            Buffer<Vector3>* normalBuffer,
            Buffer<Vector3>* depthBuffer,
            MotionBuffer* motionBuffer,
            Buffer<Vector3>* positionBuffer,
            Camera camera
        ) override {
            Buffer<Vector3> *imageBuffer2 = copyBuffer(imageBuffer);
            // Buffer<Vector3> *imageBuffer3 = copyBuffer(imageBuffer);
            Vector3 foo = imageBuffer->at(0,0);
            Buffer<Vector3> *sinOverlayBuf = new Buffer<Vector3>(imageBuffer->getWidth(), imageBuffer->getHeight());
            Effect *sinOverlay = VSin(sinOverlayBuf);
            // recommended threshold values: 
            //  bunny-scene: 0.995
            //  
            
            // for bunny scene:
            Effect *redMaskBase = ColorMultiply(Threshold(NoOp(imageBuffer2), 0.995f, AVG, false), Vector3(127.0f/255.0f, 0.0f, 0.0f));
            // otherwise: 
            // Effect *redMaskBase = ColorMultiply(Threshold(NoOp(depthBuffer), 0.995f, AVG, false), Vector3(127.0f/255.0f, 0.0f, 0.0f));
            // Effect *redMask = Multiply(sinOverlay, redMaskBase);
            // Effect *base = new Vignette(imageBuffer);
            // Effect *result = ConstMultiply(new FloydDither(new RGBConvert(new ToneMapHSV(new HSVConvert(Overlay(base, redMask))))), 255.0f);

            // Effect *redMaskBase = ColorMultiply(Threshold(NoOp(imageBuffer2), 0.905, AVG), Vector3(127.0f/255.0f, 0.0f, 0.0f));
            Effect *redMask = Multiply(sinOverlay, redMaskBase);
            Effect *base = new Vignette(imageBuffer);
            Effect *result = ConstMultiply(new FloydDither(new RGBConvert(new ToneMapHSV(new HSVConvert(Overlay(base, redMask))))), 255.0f);
            // Effect *result = ConstMultiply(new FloydDither(new RGBConvert(new ToneMapHSV(new HSVConvert(redMask)))), 255.0f);
            return new PostProcessor(result);
        }
};
