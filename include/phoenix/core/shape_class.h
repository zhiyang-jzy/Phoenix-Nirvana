#pragma once
#include "phoenix/core/common.h"
#include "phoenix/core/object_class.h"
#include "phoenix/core/common_struct.h"
namespace Phoenix{
    class Shape:public PhoenixObject{
    public:
        PClassType GetClassType()const{return PClassType::PShape;}

    };
}