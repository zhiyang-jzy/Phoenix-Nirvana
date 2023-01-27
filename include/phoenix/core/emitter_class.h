#pragma once

#include "object_class.h"
#include "common.h"
#include "common_struct.h"
#include "scene_class.h"
#include "shape_class.h"

namespace Phoenix {
    struct EmitterQueryRecord {
        Vector3f ref;
        Vector3f p;
        Vector3f n;
        Vector3f wi;
        float pdf;
        Ray shadow_ray;

        EmitterQueryRecord(const Vector3f &ref, const Vector3f &p, Vector3f n) :
                ref(ref), p(p), n(std::move(n)) {
            pdf = .0f;
            wi = (p - ref).normalized();
        }
        EmitterQueryRecord(const Vector3f& re):ref(re){}
    };


    class Emitter : public PhoenixObject {
    public:
        Color3f radiance_;

        PClassType GetClassType() const override { return PClassType::PEmitter; }

        virtual Color3f Eval(const EmitterQueryRecord &record) const = 0;
        virtual Color3f Sample(EmitterQueryRecord& rec,Vector2f sample)const = 0;

        virtual void AddToScene(Scene &scene) = 0;
    };
}
