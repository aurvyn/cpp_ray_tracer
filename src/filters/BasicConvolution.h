//
// Created by Henry Morin on 2/10/25.
//

#ifndef BASICCONVOLUTION_H
#define BASICCONVOLUTION_H

#include "Effect.h"
#include "GenVector.h"

class BasicConvolution : public Effect {
    using Effect::Effect;

    int type;
public:
    BasicConvolution *init() {
        return this;
    }

    void _apply() override
    {
        this->colorSpace = this->child->colorSpace;
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();
        for (int i=0; i<3; i++) {
            for(int y=1; y<resY-1; y++) {
                for(int x=1; x<resX-1; x++) {
                    GenVector<9> colors(0.0f);
                    int index = 0;
                    for (int j = x-1; j < x+2; j++){
                        for (int k = y-1; k < y+2; k++) {
                            colors[index] = this->imageBuffer->at(j,k)[i];
                            index++;
                        }
                    }
                    GenVector<9> blurMatrix(0.0f);
                    for (int index = 0; index < 9; index++){
                        if (index == 0 || index == 2 || index == 6 || index == 8)
                            blurMatrix[index] = 0.0625f;
                        if (index == 1 || index == 3 || index == 5 || index == 7)
                            blurMatrix[index] = 0.125f;
                        if (index == 4)
                            blurMatrix[index] = 0.25f;
                    }
                    GenVector<9> edgeMatrix(-1.0f);
                    edgeMatrix[4] = 8.0f;

                    float combined = colors.dot(edgeMatrix);
                    this->imageBuffer->at(x,y)[i] = combined;
                }
            }
        }
    }

};

#endif //BASICCONVOLUTION_H
