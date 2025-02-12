#pragma once

#include "Buffer.h"
#include "HSV.h"
#include "Effect.h"
#include <memory>
#include <vector>
#include "filters/HSVConvert.h"
#include "filters/HueShift.h"
#include "filters/LinearHSVHDR.h"
#include "filters/RGBConvert.h"
#include "filters/Add.h"
#include "filters/NoOp.h"
#include "filters/MotionBlur.h"
#include "filters/RGBMultiply.h"
#include "filters/Negative.h"
#include "filters/Bound.h"
#include "filters/Vignette.h"
#include "filters/BasicEffect.h"
#include "filters/BasicConvolution.h"

// TODO: Have apply return a Buffer<Vector3> so you can debug write each step in the pipeline to an image?
class PostProcessor
{
    Effect *root;
    public:
        PostProcessor() {}
        PostProcessor(Effect *root) {
            this->root = root;
        }

        Buffer<Vector3> *process() {
            return root->applyEffect();
        }
};

class Pipeline
{
    public:
        virtual PostProcessor* buildPipeline(
            Buffer<Vector3>* imageBuffer,
            Buffer<Vector3>* normalBuffer,
            Buffer<Vector3>* depthBuffer,
            MotionBuffer motionBuffer = MotionBuffer(0, 0)
        ) = 0;
};

class DefaultPipeline: public Pipeline
{
    public:
        // make && ./build/tracer {scene}
        // PostProcessor is responsible for making sure the image is in RGB255 format
        PostProcessor* buildPipeline(
            Buffer<Vector3>* imageBuffer,
            Buffer<Vector3>* normalBuffer,
            Buffer<Vector3>* depthBuffer,
            MotionBuffer motionBuffer
        ) override {
            // Effect *effect = (new RGBMultiply(new Negative(new RGBConvert(new LinearHSVHDR(
            //     (new HueShift(new HSVConvert(
            //     (new MotionBlur(imageBuffer))->init(motionBuffer)
            //     )))->init(60.0f)
            //     )))))->init(255.0f);
            // Effect *effect = (new RGBConvert((new Bound(new HSVConvert(imageBuffer)))->init(900.0f, 1500.0f, 2)));
            // Effect *effect = (new RGBMultiply (new RGBConvert (new LinearHSVHDR(new HSVConvert((new BasicConvolution (new NoOp(imageBuffer)))->init(2))))))->init(255.0f);
            // Effect *effect = (new RGBMultiply (new RGBConvert (new LinearHSVHDR( new HSVConvert((new NoOp(imageBuffer)))))))->init(255.0f);
            // Effect *effect = new BasicConvolution(new NoOp(imageBuffer));
            Effect *effect = (new RGBMultiply(new RGBConvert(new LinearHSVHDR(new HSVConvert(depthBuffer)))))->init(255.0f);
            // Effect *effect = new NoOp(imageBuffer);
            return new PostProcessor(effect);
        }
};
