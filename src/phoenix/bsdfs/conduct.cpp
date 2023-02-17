#include "phoenix/core/bsdf_class.h"

namespace Phoenix {
    class Conduct : public Bsdf {
    public:
        explicit Conduct(PropertyList propers) {
            base_color_ = make_shared<SingleColorTexture>(Color3f(1, 1, 1), "basecolor");
            is_two_sided_ = false;
        }

        [[nodiscard]] string ToString() const override { return "conduct"; }

        Color3f Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const override {
            rec.wo = Vector3f(-rec.wi.x(), -rec.wi.y(), rec.wi.z());
            pdf = 1.f;
            return base_color_->GetColor(rec.uv);
        }

        Color3f Eval(const BSDFQueryRecord &rec) const override {
            auto wo = Vector3f(-rec.wi.x(), -rec.wi.y(), rec.wi.z());
            if (rec.wo.isApprox(wo))
                return Color3f(1.0);
            return {0, 0, 0};

        }

        float Pdf(const BSDFQueryRecord &rec) const override {
            auto wo = Vector3f(-rec.wi.x(), -rec.wi.y(), rec.wi.z());
            if (rec.wo.isApprox(wo))
                return 1.f;
            return 0.f;
        }

        bool IsSpecular() const override {
            return true;
        }
    };

    PHOENIX_REGISTER_CLASS(Conduct, "conductor");
}