#pragma once
#include "common.h"
#include "object_class.h"
#include "scene_class.h"
#include "sampler_class.h"
#include "common_struct.h"
#include "shape_class.h"

namespace Phoenix{
    class Integrator:public PhoenixObject{
    public:
        [[nodiscard]] PClassType GetClassType()const override{
            return PClassType::PIntegrator;
        }
        virtual Vector3f Li(shared_ptr<Scene> scene,shared_ptr<Sampler> sampler,const Ray& ray)=0;
    };
}