#include "phoenix/core/microfacet_class.h"
#include "phoenix/core/bsdf_class.h"
#include "phoenix/core/utils.h"

namespace Phoenix {
    class RoughConductor : public Bsdf {
    private:
        float alpha_;
        Color3f specular_reflectance_;
        Color3f eta_, k_;
        shared_ptr<MicrofacetDistribution> distribution_;

    public:
        RoughConductor(PropertyList propers) {
            alpha_ = propers.Get<float>("alpha").value();
            base_color_ = propers.Get<vector<float>>("specularReflectance").value();
            eta_ = propers.Get<vector<float>>("eta").value();
            k_ = propers.Get<vector<float>>("k").value();
            specular_reflectance_ = base_color_;
            distribution_ = make_shared<MicrofacetDistribution>(alpha_);
        }

        [[nodiscard]] string ToString() const override { return "rough_conductor"; }

        void Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const override {
            if (Frame::CosTheta(rec.wi) < 0){
                pdf = 0.f;
                return;
            }


            Normal3f m = distribution_->Sample(rec.wi, sample, pdf);

            if (pdf == 0)
                return ;

            /* Perfect specular reflection based on the microfacet normal */
            rec.wo = Reflect(rec.wi, m);

            /* Side check */
            if (Frame::CosTheta(rec.wo) <= 0)
            {
                pdf = 0.f;
                return;
            }


        }

        Color3f Eval(const BSDFQueryRecord &rec) const override {
            if (Frame::CosTheta(rec.wi) <= 0 || Frame::CosTheta(rec.wo) <= 0)
                return Color3f(0.0f);

            /* Calculate the reflection half-vector */
            Vector3f H = (rec.wo + rec.wi).normalized();


            /* Evaluate the microfacet normal distribution */
            const float D = distribution_->Eval(H);
            if (D == 0)
                return Color3f(0.0f);

            /* Fresnel factor */
            const Color3f F = FresnelConductorExact(rec.wi.dot(H), eta_, k_) *
                              specular_reflectance_;

            /* Smith's shadow-masking function */
            const float G = distribution_->G(rec.wi, rec.wo, H);

            /* Calculate the total amount of reflection */
            float model = D * G / (4.0f * Frame::CosTheta(rec.wi));

            return F * model;

        }

        float Pdf(const BSDFQueryRecord &rec) const override {
            if (Frame::CosTheta(rec.wi) <= 0 || Frame::CosTheta(rec.wo) <= 0)
                return 0.0f;

            /* Calculate the reflection half-vector */
            Vector3f H = (rec.wo + rec.wi).normalized();

            return distribution_->Eval(H) * distribution_->SmithG1(rec.wi, H)
                   / (4.0f * Frame::CosTheta(rec.wi));

        }
    };

    PHOENIX_REGISTER_CLASS(RoughConductor, "roughconductor");
}