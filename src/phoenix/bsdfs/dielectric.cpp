#include "phoenix/core/bsdf_class.h"

namespace Phoenix {


    class Dielectric : public Bsdf {

    private:

        [[nodiscard]] Vector3f GetOutgoingVector(const BSDFQueryRecord &bRec, bool returnRefract) const {
            Vector3f n(0, 0, 1);
            float n1 = ext_ior_;
            float n2 = int_ior_;

            float cosT = bRec.wi.z();

            if (cosT < 0) {
                n1 = int_ior_;
                n2 = ext_ior_;
                n = -n;
                cosT = -cosT;
            }

            float snell = n1 / n2;

            Vector3f part1 = -snell * (bRec.wi - cosT * n);
            float cons = sqrt(1.0f - (snell * snell) * (1.0f - cosT * cosT));
            Vector3f part2 = n * cons;

            Vector3f refraction = part1 - part2;
            refraction = refraction.normalized();

            Vector3f reflection = Vector3f(-bRec.wi.x(), -bRec.wi.y(), bRec.wi.z());

            if (returnRefract)
                return refraction;
            else
                return reflection;
        }

        [[nodiscard]] Vector3f GetReflection(const BSDFQueryRecord &bRec) const {
            return GetOutgoingVector(bRec, false);
        }

        [[nodiscard]] Vector3f GetRefraction(const BSDFQueryRecord &bRec) const {
            return GetOutgoingVector(bRec, true);
        }

    private:
        float int_ior_, ext_ior_;
    public:
        explicit Dielectric(PropertyList propers) {
            int_ior_ = propers.Get<float>("intIOR").value_or(1.f);
            ext_ior_ = propers.Get<float>("extIOR").value_or(1.5f);


            base_color_ = Color3f(1, 1, 1);
        }

        [[nodiscard]] string ToString() const override { return "conduct"; }

        void Sample(BSDFQueryRecord &rec, float &pdf, const Vector2f &sample) const override {
            Vector3f n(0, 0, 1);
            float n1 = ext_ior_;
            float n2 = int_ior_;

            float cosT = rec.wi.z();

            if (cosT < 0) {
                n1 = int_ior_;
                n2 = ext_ior_;
                n = -n;
                cosT = -cosT;
            }

            float F = Fresnel(cosT, n1, n2);
            float snell = n1 / n2;

            float cons = sqrt(1.0f - (snell * snell) * (1.0f - cosT * cosT));

            bool isTReflec = cons > 1;

            if (isTReflec) {
                rec.wo = GetReflection(rec).normalized();
                pdf = 1.f;
            } else {
                if (sample.x() < F) {
                    rec.wo = GetReflection(rec).normalized();
                    pdf = F;
                } else {
                    rec.wo = GetRefraction(rec).normalized();
                    pdf = 1 - F;
                }
            }
        }

        Color3f Eval(const BSDFQueryRecord &rec) const override {
            Vector3f reflec = GetReflection(rec);
            Vector3f refrac = GetRefraction(rec);

            bool isRefl = rec.wo.isApprox(reflec);
            bool isRefr = rec.wo.isApprox(refrac);

            float n1 = ext_ior_;
            float n2 = int_ior_;

            float cosT = rec.wi.z();

            if (cosT < 0) {
                n1 = int_ior_;
                n2 = ext_ior_;
            }
            float snell = n1 / n2;
            float F = Fresnel(cosT, n1, n2);

            float cons = sqrt(1.0f - (snell * snell) * (1.0f - cosT * cosT));

            bool isTReflec = cons > 1;

            if (isTReflec) {
                if (isRefl) {
                    return {1, 1, 1};
                }
                return {0, 0, 0};
            } else {
                if (isRefl)
                    return Color3f(1.f) * F;
                else if (isRefr)
                    return snell * snell * Color3f(1.f) * (1 - F);
                else
                    return {0, 0, 0};
            }


        }

        float Pdf(const BSDFQueryRecord &rec) const override {
            Vector3f n(0, 0, 1);
            float n1 = ext_ior_;
            float n2 = int_ior_;

            float cosT = rec.wi.z();

            if (cosT < 0) {
                n1 = int_ior_;
                n2 = ext_ior_;
                n = -n;
                cosT = -cosT;
            }

            float F = Fresnel(cosT, n1, n2);
            float snell = n1 / n2;

            Vector3f refrac = GetRefraction(rec);
            Vector3f reflec = GetReflection(rec);
            bool isRefl = rec.wo.isApprox(reflec);
            bool isRefr = rec.wo.isApprox(refrac);


            float cons = sqrt(1.0f - (snell * snell) * (1.0f - cosT * cosT));

            bool isTReflec = cons > 1;

            if (isTReflec) {
                if (isRefl) {
                    return 1.f;
                }
                return 0.f;
            } else {
                if (isRefl)
                    return F;
                else if (isRefr)
                    return 1 - F;
                else
                    return 0.f;
            }
        }

        bool IsSpecular() const override {
            return true;
        }
    };

    PHOENIX_REGISTER_CLASS(Dielectric, "dielectric");

}