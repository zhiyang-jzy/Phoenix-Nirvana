#include "phoenix/core/bsdf_class.h"
#include "phoenix/core/utils.h"

namespace Phoenix {


    class SmoothPlastic : public Bsdf {
    private:
        float int_ior_, ext_ior_;
        shared_ptr<Texture> specular_reflectance_, diffuse_reflectance_;
        bool has_specular_, has_diffuse_;
        float m_specularSamplingWeight;
        float eta_, fdr_int_, fdr_ext_;
        float inv_eta2_;

    public:
        explicit SmoothPlastic(PropertyList properties) {
            specular_reflectance_ = nullptr;
            diffuse_reflectance_ = nullptr;

            int_ior_ = properties.Get<float>("intIOR").value_or(1.f);
            ext_ior_ = properties.Get<float>("extIOR").value_or(1.5f);
            eta_ = int_ior_ / ext_ior_;
            fdr_int_ = fresnelDiffuseReflectance(1. / eta_);
            fdr_ext_ = fresnelDiffuseReflectance(eta_);
            inv_eta2_ = 1.f / (eta_ * eta_);

        }

        [[nodiscard]] string ToString() const override { return "rough plastic"; }

        Color3f Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const override {
            if (Frame::CosTheta(rec.wi) <= 0)
                return Color3f(0.0f);

            float Fi = Fresnel(Frame::CosTheta(rec.wi), ext_ior_, int_ior_);

            float probSpecular = (Fi * m_specularSamplingWeight) /
                                 (Fi * m_specularSamplingWeight +
                                  (1 - Fi) * (1 - m_specularSamplingWeight));

            /* Importance sample wrt. the Fresnel reflectance */
            if (sample.x() < probSpecular) {
                rec.wo = Reflect(rec.wi);
                pdf = probSpecular;

                return specular_reflectance_->GetColor(rec.uv)
                       * Fi / probSpecular;
            } else {
                rec.wo = SquareToCosineHemisphere(Vector2f(
                        (sample.x() - probSpecular) / (1 - probSpecular),
                        sample.y()));
                float Fo = Fresnel(Frame::CosTheta(rec.wo), ext_ior_, int_ior_);

                Color3f diff = diffuse_reflectance_->GetColor(rec.uv);
                diff /= 1 - fdr_int_;

                pdf = (1 - probSpecular) *
                      SquareToCosineHemispherePdf(rec.wo);

                return diff * (inv_eta2_ * (1 - Fi) * (1 - Fo) / (1 - probSpecular));
            }


        };

        Color3f Eval(const BSDFQueryRecord &rec) const override {

            if (Frame::CosTheta(rec.wo) <= 0 || Frame::CosTheta(rec.wi) <= 0)
                return {0, 0, 0};


            float Fi = Fresnel(Frame::CosTheta(rec.wi), ext_ior_, int_ior_);


            if (Reflect(rec.wi).isApprox(rec.wo))
                return specular_reflectance_->GetColor(rec.uv) * Fi;

            float Fo = Fresnel(Frame::CosTheta(rec.wo), ext_ior_, int_ior_);
            Color3f diff = diffuse_reflectance_->GetColor(rec.uv);
            diff /= 1 - fdr_int_;

            return diff * (SquareToCosineHemispherePdf(rec.wo)
                           * inv_eta2_ * (1 - Fi) * (1 - Fo));

        }

        float Pdf(const BSDFQueryRecord &rec) const override {

            if (Frame::CosTheta(rec.wo) <= 0 || Frame::CosTheta(rec.wi) <= 0)
                return 0.f;
            float probSpecular = 1.f;
            float Fi = Fresnel(Frame::CosTheta(rec.wi), ext_ior_, int_ior_);
            probSpecular = (Fi * m_specularSamplingWeight) /
                           (Fi * m_specularSamplingWeight +
                            (1 - Fi) * (1 - m_specularSamplingWeight));


            if (rec.wo.isApprox(Reflect(rec.wi))) {
                return probSpecular;
            } else {
                return SquareToCosineHemispherePdf(rec.wo) * (1 - probSpecular);
            }

        }

        void AddChild(shared_ptr<PhoenixObject> child) override {
            if (child->GetClassType() == PhoenixObject::PClassType::PTexture) {
                auto texture = std::dynamic_pointer_cast<Texture>(child);
                if (texture->name() == "diffuseReflectance") {
                    diffuse_reflectance_ = texture;
                    base_color_ = diffuse_reflectance_;
                } else if (texture->name() == "specularReflectance") {
                    specular_reflectance_ = texture;
                }
            }
        }

        void Active() override {
            if (!diffuse_reflectance_) {
                diffuse_reflectance_ = make_shared<SingleColorTexture>(Color3f(0.5, 0.5, 0.5),"");
            }
            if (!specular_reflectance_)
                specular_reflectance_ = make_shared<SingleColorTexture>(Color3f(1, 1, 1),"");
            base_color_ = diffuse_reflectance_;

            float d_ave = diffuse_reflectance_->GetAverage().getLuminance(),
                    s_ave = specular_reflectance_->GetAverage().getLuminance();
            m_specularSamplingWeight = s_ave / (d_ave + s_ave);

        }

        bool IsSpecular()const override {
            return false;
        }

    };

    PHOENIX_REGISTER_CLASS(SmoothPlastic, "plastic");
}