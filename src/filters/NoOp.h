#pragma once

#include "Effect.h"

class NoOp : public Effect
{
    using Effect::Effect;

    public:
        void _apply() override
        { }
};

