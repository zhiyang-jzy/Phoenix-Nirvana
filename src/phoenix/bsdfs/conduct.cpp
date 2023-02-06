#include "phoenix/core/bsdf_class.h"

namespace Phoenix {
    class Conduct : public Bsdf {
    public:
        explicit Conduct(PropertyList propers) {
            base_color_ = {1,1,1};
        }

        [[nodiscard]] string ToString() const override { return "conduct"; }

        void Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const override {
            if (rec.wi.z() <= 0) {
                pdf = 0;
                return;
            }
            rec.wo = Vector3f(-rec.wi.x(), -rec.wi.y(), rec.wi.z());
            pdf = 1.f;
        }
        Color3f Eval(const BSDFQueryRecord &rec) const override{
            auto wo = Vector3f(-rec.wi.x(), -rec.wi.y(), rec.wi.z());
            if(rec.wo.isApprox(wo))
                return {1,1,1};
            return {0,0,0};

        }
        float Pdf(const BSDFQueryRecord &rec) const override{
            auto wo = Vector3f(-rec.wi.x(), -rec.wi.y(), rec.wi.z());
            if(rec.wo.isApprox(wo))
                return 1.f;
            return 0.f;
        }
    };

    PHOENIX_REGISTER_CLASS(Conduct,"conduct");
}