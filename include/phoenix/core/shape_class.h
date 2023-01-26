#pragma once
#include "phoenix/core/common.h"
#include "phoenix/core/object_class.h"
#include "phoenix/core/common_struct.h"


namespace Phoenix{
    class Scene;
    class Shape:public PhoenixObject{
    private:
    public:
        Vector3f base_color_;
        PClassType GetClassType()const{return PClassType::PShape;}
        virtual void AddToScene(Scene &scene) =0;
        virtual Vector3f GetColor(const TracerHit& hit)=0;

    };
}