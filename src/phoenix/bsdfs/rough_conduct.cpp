#include "phoenix/core/microfacet_class.h"
#include "phoenix/core/bsdf_class.h"
#include "phoenix/core/utils.h"

namespace Phoenix {
    class RoughConductor : public Bsdf {
    private:
        float alpha_;
        Color3f specular_reflectance_;
        Color3f eta_, k_;

    public:
        RoughConductor(PropertyList propers) {
            alpha_ = propers.Get<float>("alpha").value_or(1.f);
            base_color_ = propers.Get<vector<float>>("specularReflectance").value_or(vector<float>{1, 1, 1});
            eta_ = propers.Get<vector<float>>("eta").value_or(vector<float>{1, 1, 1});
            k_ = propers.Get<vector<float>>("k").value_or(vector<float>{1, 1, 1});
            specular_reflectance_ = base_color_;
        }

        [[nodiscard]] string ToString() const override { return "rough_conductor"; }

        void Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const override {
            MicrofacetDistribution distribution(alpha_);
            Normal3f m = distribution.Sample(rec.wi, sample, pdf);
            if (pdf == 0)
                return;
            rec.wo = Reflect(rec.wi, m);
        }

        Color3f Eval(const BSDFQueryRecord &rec) const override {
            if (Frame::CosTheta(rec.wi) <= 0 || Frame::CosTheta(rec.wo) <= 0)
                return {0, 0, 0};
            auto H = (rec.wi + rec.wo).normalized();
            MicrofacetDistribution distribution(alpha_);
            float D = distribution.Eval(H);
            if (D == 0)
                return {0, 0, 0};
            Color3f F = FresnelConductorExact(rec.wi.dot(H), eta_, k_) * specular_reflectance_;
            auto G = distribution.G(rec.wi, rec.wo, H);
            float model = D * G / (4.0f * Frame::CosTheta(rec.wi));
            return F * model;

        }

        float Pdf(const BSDFQueryRecord &rec) const override {
            if (Frame::CosTheta(rec.wi) <= 0 || Frame::CosTheta(rec.wo) <= 0) {
                return 0.f;
            }
            auto H = (rec.wi + rec.wo).normalized();
            MicrofacetDistribution distribution(alpha_);
            return distribution.Eval(H) * distribution.SmithG1(rec.wi, H)
                   / (4.0f * Frame::CosTheta(rec.wi));

        }
    };

    PHOENIX_REGISTER_CLASS(RoughConductor, "roughconductor");
}