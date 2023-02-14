#include "phoenix/core/bsdf_class.h"
#include "phoenix/core/utils.h"

namespace Phoenix {


    class SmoothPlastic : public Bsdf {
    private:
        float int_ior_, ext_ior_;
        Color3f specular_reflectance_, diffuse_reflectance_;
        bool has_specular_, has_diffuse_;
        float m_specularSamplingWeight;
        float eta_, fdr_int_, fdr_ext_;
        float inv_eta2_;

    public:
        explicit SmoothPlastic(PropertyList properties) {
            base_color_ = properties.Get<vector<float>>("diffuseReflectance").value_or(vector<float>{1, 1, 1});
            specular_reflectance_ = properties.Get<vector<float>>("specularReflectance").value_or(
                    vector<float>{1, 1, 1});
            diffuse_reflectance_ = properties.Get<vector<float>>("diffuseReflectance").value_or(
                    vector<float>{0.5, 0.5, 0.5});

            int_ior_ = properties.Get<float>("intIOR").value_or(1.f);
            ext_ior_ = properties.Get<float>("extIOR").value_or(1.5f);
            m_specularSamplingWeight = specular_reflectance_.getLuminance() /
                                       (specular_reflectance_.getLuminance() + diffuse_reflectance_.getLuminance());
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

                return specular_reflectance_
                       * Fi / probSpecular;
            } else {
                rec.wo = SquareToCosineHemisphere(Vector2f(
                        (sample.x() - probSpecular) / (1 - probSpecular),
                        sample.y()));
                float Fo = Fresnel(Frame::CosTheta(rec.wo), ext_ior_, int_ior_);

                Color3f diff = diffuse_reflectance_;
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
                return specular_reflectance_ * Fi;

            float Fo = Fresnel(Frame::CosTheta(rec.wo), ext_ior_, int_ior_);
            Color3f diff = diffuse_reflectance_;
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

    };

    PHOENIX_REGISTER_CLASS(SmoothPlastic, "plastic");
}