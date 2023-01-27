#include "phoenix/core/bsdf_class.h"

namespace Phoenix {


    class Diffuse : public Bsdf {
    private:
    public:
        Diffuse(PropertyList properties) {
            base_color_ = properties.Get<Vector3f>("basecolor").value_or(Vector3f(1, 1, 1));

        }

        [[nodiscard]] string ToString() const override { return "diffuse"; }

        Color3f Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const override {
            if (rec.wi.z() < 0)
                return {0, 0, 0};
            rec.wo = SquareToCosineHemisphere(sample);
            pdf = SquareToCosineHemispherePdf(rec.wo);
            return base_color_;
        };

        float Eval(const BSDFQueryRecord &rec) override {
            float v = kInvPi*Frame::CosTheta(rec.wo);
            return v;

        }


    };

    PHOENIX_REGISTER_CLASS(Diffuse, "diffuse");
}