#pragma once

#include "phoenix/core/common.h"
#include "phoenix/core/object_class.h"
#include "phoenix/core/common_struct.h"
#include "tracer_class.h"


#include "bsdf_class.h"

namespace Phoenix {
    class Scene;

    class Shape : public PhoenixObject {
    private:
        shared_ptr<Bsdf> bsdf_;
    public:
        float area_, inv_area_;

        [[nodiscard]] PClassType GetClassType() const override { return PClassType::PShape; }

        virtual vector<uint> AddToTracer(Tracer &tracer) = 0;

        virtual shared_ptr<Bsdf> bsdf() { return bsdf_; }

        virtual float area() { return area_; }

        virtual void SamplePosition(PositionSampleRecord &rec, Vector2f sample) = 0;

        virtual Color3f base_color(Vector2f uv) { return bsdf_->base_color(uv); }

        virtual Vector2f GetUv(const Interaction &its) const = 0;

        void AddChild(shared_ptr<PhoenixObject> child) override {
            bsdf_ = std::dynamic_pointer_cast<Bsdf>(child);
        }

        virtual void ApplyTransform(const Transform &trans) = 0;

        virtual void SampleDirect(DirectSamplingRecord &dRec, const Vector2f &sample);

        virtual float PdfPosition(const PositionSampleRecord &pRec) const = 0;

        virtual float PdfDirect(const DirectSamplingRecord &dRec) ;


    };
}