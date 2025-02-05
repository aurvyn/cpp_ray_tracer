#pragma once

#include "Effect.h"

class NoOp : public Effect
{
    using Effect::Effect;

    public:
        void _apply() override {
            this->colorSpace = this->child->colorSpace;
        }
};

