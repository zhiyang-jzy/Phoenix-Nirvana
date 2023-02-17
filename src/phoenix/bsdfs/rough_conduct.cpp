#include "phoenix/core/microfacet_class.h"
#include "phoenix/core/bsdf_class.h"
#include "phoenix/core/utils.h"

namespace Phoenix {
    class RoughConductor : public Bsdf {
    private:
        float alpha_;
        shared_ptr<Texture> specular_reflectance_;
        shared_ptr<Texture> eta_, k_;
        shared_ptr<MicrofacetDistribution> distribution_;

    public:
        RoughConductor(PropertyList propers) {
            alpha_ = propers.Get<float>("alpha").value();
            distribution_ = make_shared<MicrofacetDistribution>(alpha_);
            is_two_sided_ = propers.Get<bool>("twosided").value_or(true);
        }

        [[nodiscard]] string ToString() const override { return "rough_conductor"; }

        Color3f Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const override {
            if (Frame::CosTheta(rec.wi) < 0) {
                return {0, 0, 0};
            }


            Normal3f m = distribution_->Sample(rec.wi, sample, pdf).normalized();

            if (pdf == 0)
                return {0, 0, 0};

            /* Perfect specular reflection based on the microfacet normal */
            rec.wo = Reflect(rec.wi, m).normalized();

            /* Side check */
            if (Frame::CosTheta(rec.wo) <= 0) {
                return {0, 0, 0};
            }
            const Color3f F = FresnelConductorExact(rec.wi.dot(m), eta_->GetColor(rec.uv), k_->GetColor(rec.uv)) *
                              specular_reflectance_->GetColor(rec.uv);
            float weight = distribution_->SmithG1(rec.wo, m);
            pdf /= (4.0f * rec.wo.dot(m));
            return F * weight;


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
            const Color3f F = FresnelConductorExact(rec.wi.dot(H), eta_->GetColor(rec.uv), k_->GetColor(rec.uv)) *
                              specular_reflectance_->GetColor(rec.uv);

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

        void AddChild(shared_ptr<PhoenixObject> child) override {
            if (child->GetClassType() == PhoenixObject::PClassType::PTexture) {
                auto texture = std::dynamic_pointer_cast<Texture>(child);
                if (texture->name() == "specularReflectance") {
                    specular_reflectance_ = texture;
                    base_color_ = specular_reflectance_;
                } else if (texture->name() == "eta") {
                    eta_ = texture;
                } else if (texture->name() == "k") {
                    k_ = texture;
                }

            }
        }

        bool IsSpecular()const override {
            return false;
        }
    };

    PHOENIX_REGISTER_CLASS(RoughConductor, "roughconductor");
}