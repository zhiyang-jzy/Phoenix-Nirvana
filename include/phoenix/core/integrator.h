#pragma once
#include "common.h"
#include "object_class.h"
#include "scene_class.h"
#include "sampler_class.h"
#include "common_struct.h"
#include "shape_class.h"
#include "emitter_class.h"
#include "bsdf_class.h"

namespace Phoenix{
    class Integrator:public PhoenixObject{
    public:
        [[nodiscard]] PClassType GetClassType()const override{
            return PClassType::PIntegrator;
        }
        [[nodiscard]] virtual Color3f Li(shared_ptr<Scene> scene, shared_ptr<Sampler> sampler, const Ray& ray)const =0;
    };
}