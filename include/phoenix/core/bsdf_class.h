#pragma once

#include <utility>

#include "common.h"
#include "object_class.h"
#include "sampler_class.h"
#include "common_struct.h"
#include "texture_class.h"

namespace Phoenix {

    struct BSDFQueryRecord {
        Vector3f wi;
        Vector3f wo;
        Vector2f uv;

        BSDFQueryRecord(Vector3f _wi, Vector2f _uv)
                : wi(std::move(_wi)), uv(std::move(_uv)) {}

        BSDFQueryRecord(Vector3f _wi, Vector3f _wo, Vector2f _uv)
                : wi(std::move(_wi)), wo(std::move(_wo)), uv(std::move(_uv)) {}
    };

    class Bsdf : public PhoenixObject {
    public:
        shared_ptr<Texture> base_color_;
        bool is_two_sided_;

        [[nodiscard]] PClassType GetClassType() const override { return PClassType::PBSDF; }

        virtual Color3f Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const = 0;

        virtual Color3f Eval(const BSDFQueryRecord &rec) const = 0;

        virtual float Pdf(const BSDFQueryRecord &rec) const = 0;

        virtual Color3f base_color(const Vector2f uv) const { return base_color_->GetColor(uv); }

        virtual bool IsSpecular() const { return false; }

        virtual bool IsTwoSided() const { return is_two_sided_; }


    };
}
