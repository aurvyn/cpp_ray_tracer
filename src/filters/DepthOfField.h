//
// Created by Henry Morin on 2/12/25.
//

#ifndef DEPTHOFFIELD_H
#define DEPTHOFFIELD_H

#include "Effect.h"
#include "Buffer.h"
#include "GenVector.h"
#include "Bound.h"
#include "ToneMapHSV.h"
#include "HSVConvert.h"
#include "RGBConvert.h"
#include "BasicEffect.h"
#include "BasicConvolution.h"

class DepthOfField : public BlendMode {
    using BlendMode::BlendMode;

    float focalPoint = 0.59f;
    float focalWidth = 0.07f;
public:
    DepthOfField *init(float focalPoint, float focalWidth) {
        this->focalPoint = focalPoint;
        this->focalWidth = focalWidth;
        return this;
    }

    void _apply() override
    {
        this->colorSpace = this->child->colorSpace;
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();

        Buffer<Vector3> near (*imageBuffer2);
        Buffer<Vector3> focus (*imageBuffer2);
        Buffer<Vector3> far (*imageBuffer2);

        Effect* nearFiltered = new RGBConvert((new Bound(new ToneMapHSV(new HSVConvert(&near))))->init(0.0f, focalPoint-focalWidth, 2));
        nearFiltered->applyEffect();
        Effect* focusFiltered = new RGBConvert((new Bound(new ToneMapHSV(new HSVConvert(&focus))))->init(focalPoint-focalWidth, focalPoint+focalWidth, 2));
        focusFiltered->applyEffect();
        Effect* farFiltered = new RGBConvert((new Bound(new ToneMapHSV(new HSVConvert(&far))))->init(focalPoint+focalWidth, 1.0f, 2));
        farFiltered->applyEffect();

        for (int y = 0; y < resY; y++){
            for (int x = 0; x < resX; x++){
                if (near.at(x,y).squaredLength() != 0) {
                    near.at(x,y) = Vector3(1.0f);
                }
                if (focus.at(x,y).squaredLength() != 0) {
                    focus.at(x,y) = Vector3(1.0f);
                }
                if (far.at(x,y).squaredLength() != 0) {
                    far.at(x,y) = Vector3(1.0f);
                }
            }
        }

        Buffer<Vector3> nearImage (*imageBuffer);
        Buffer<Vector3> focusImage (*imageBuffer);
        Buffer<Vector3> farImage (*imageBuffer);

        Effect* nearImageBlur = Multiply(NoOp(&nearImage), NoOp(&near));
        nearImageBlur->applyEffect();
        Effect* focusImageBlur = Multiply((new BasicConvolution(NoOp(&focusImage)))->init(2), NoOp(&focus));
        focusImageBlur->applyEffect();
        Effect* farImageBlur = Multiply((new BasicConvolution(NoOp(&farImage)))->init(5), NoOp(&far));
        farImageBlur->applyEffect();

        Effect* image = Add(NoOp(&nearImage), Add(NoOp(&focusImage), NoOp(&farImage)));
        image->applyEffect();

        for (int y = 0; y < resY; y++){
            for (int x = 0; x < resX; x++){
                this->imageBuffer->at(x,y) = nearImage.at(x,y);
            }
        }

    }

};

#endif //DEPTHOFFIELD_H
