#pragma once
#include "common.h"
#include "object_class.h"
#include "sampler_class.h"
#include "common_struct.h"
namespace Phoenix{

    struct BSDFQueryRecord {
        Vector3f wi;
        Vector3f wo;
        explicit BSDFQueryRecord(Vector3f wi)
                : wi(std::move(wi)) {}

        BSDFQueryRecord(Vector3f wi,
                        Vector3f wo)
                : wi(std::move(wi)), wo(std::move(wo)) {}
    };

    class Bsdf:public PhoenixObject{
    public:
        Color3f base_color_;
        [[nodiscard]] PClassType GetClassType() const override{return PClassType::PBSDF;}
        virtual void Sample(BSDFQueryRecord& rec, float& pdf, const Vector2f& sample)const = 0;
        virtual Color3f Eval(const BSDFQueryRecord& rec)=0;

    };
}
