#include "phoenix/core/bsdf_class.h"

namespace Phoenix {


    class Diffuse : public Bsdf {
    private:
    public:
        explicit Diffuse(PropertyList properties) {
            base_color_ = properties.Get<Vector3f>("basecolor").value_or(Vector3f(1, 1, 1));

        }

        [[nodiscard]] string ToString() const override { return "diffuse"; }

        void Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const override {
            rec.wo = SquareToCosineHemisphere(sample);
            pdf = SquareToCosineHemispherePdf(rec.wo);
        };

        Color3f Eval(const BSDFQueryRecord &rec)const  override {
            if (Frame::CosTheta(rec.wo) < 0 || Frame::CosTheta(rec.wi) < 0)
                return 0.f;
            return kInvPi * base_color_*Frame::CosTheta(rec.wo);

        }
        float Pdf(const BSDFQueryRecord &rec) const override{
            if (Frame::CosTheta(rec.wo) < 0 || Frame::CosTheta(rec.wi) < 0)
                return 0.f;
            return SquareToCosineHemispherePdf(rec.wo);
        }


    };

    PHOENIX_REGISTER_CLASS(Diffuse, "diffuse");
}