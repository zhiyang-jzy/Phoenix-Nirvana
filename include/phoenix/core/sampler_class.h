#pragma once
#include "phoenix/core/common.h"
#include "phoenix/core/object_class.h"

namespace Phoenix{
    class Sampler:public PhoenixObject{
    public:
        virtual float Next1D()=0;
        virtual Vector2f Next2D()=0;
        PClassType GetClassType() const final{return PClassType::PSampler;}

    };
}