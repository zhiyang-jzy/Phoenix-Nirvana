#include "phoenix/core/microfacet_class.h"
#include "phoenix/core/utils.h"

namespace Phoenix {

    float MicrofacetDistribution::Eval(const Vector3f &m) {
        if (Frame::CosTheta(m) <= 0)
            return 0.0f;

        float cosTheta2 = Frame::cosTheta2(m);
        float beckmannExponent = ((m.x() * m.x()) / (alpha_ * alpha_)
                                  + (m.y() * m.y()) / (alpha_ * alpha_)) / cosTheta2;

        float result;
        /* GGX / Trowbridge-Reitz distribution function for rough surfaces */
        float root = ((float) 1 + beckmannExponent) * cosTheta2;
        result = (float) 1 / (kPi * alpha_ * alpha_ * root * root);


        /* Prevent potential numerical issues in other stages of the model */
        if (result * Frame::CosTheta(m) < 1e-20f)
            result = 0;

        return result;
    }

    Normal3f MicrofacetDistribution::Sample(const Vector3f &wi_, const Vector2f &sample, float &pdf) {
        Normal3f m;
        m = SampleVisible(wi_, sample).normalized();
        pdf = Pdf(wi_, m);
        return m;


    }

    inline float sig_num(float x) {
        return (x > 0) - (x < 0);
    }

    Vector2f MicrofacetDistribution::Sample11(float thetaI, Vector2f sample) const {
        Vector2f slope;
        /* Special case (normal incidence) */
        if (thetaI < 1e-4f) {
            float sinPhi, cosPhi;
            float r = safe_sqrt(sample.x() / (1 - sample.x()));
            sincos(2 * kPi * sample.y(), &sinPhi, &cosPhi);
            return Vector2f(r * cosPhi, r * sinPhi);
        }

        /* Precomputations */
        float tanThetaI = std::tan(thetaI);
        float a = 1.f / tanThetaI;
        float G1 = 2.0f / (1.0f + safe_sqrt(1.0f + 1.0f / (a * a)));

        /* Simulate X component */
        float A = 2.0f * sample.x() / G1 - 1.0f;
        if (std::abs(A) == 1)
            A -= sig_num(A) * kEpsilon;
        float tmp = 1.0f / (A * A - 1.0f);
        float B = tanThetaI;
        float D = safe_sqrt(B * B * tmp * tmp - (A * A - B * B) * tmp);
        float slope_x_1 = B * tmp - D;
        float slope_x_2 = B * tmp + D;
        slope.x() = (A < 0.0f || slope_x_2 > 1.0f / tanThetaI) ? slope_x_1 : slope_x_2;

        /* Simulate Y component */
        float S;
        if (sample.y() > 0.5f) {
            S = 1.0f;
            sample.y() = 2.0f * (sample.y() - 0.5f);
        } else {
            S = -1.0f;
            sample.y() = 2.0f * (0.5f - sample.y());
        }

        /* Improved fit */
        float z =
                (sample.y() * (sample.y() * (sample.y() * (-(float) 0.365728915865723) + (float) 0.790235037209296) -
                               (float) 0.424965825137544) + (float) 0.000152998850436920) /
                (sample.y() *
                 (sample.y() * (sample.y() * (sample.y() * (float) 0.169507819808272 - (float) 0.397203533833404) -
                                (float) 0.232500544458471) + (float) 1) - (float) 0.539825872510702);

        slope.y() = S * z * std::sqrt(1.0f + slope.x() * slope.x());
        return slope;


    }

    float MicrofacetDistribution::Pdf(const Vector3f &wi, const Normal3f &m) {
        if (Frame::CosTheta(wi) == 0)
            return 0.0f;
        auto a = SmithG1(wi, m);
        auto b = abs(wi.dot(m));
        auto c = Eval(m);
        auto d = std::abs(Frame::CosTheta(wi));
        return a * b * c / d;
        return SmithG1(wi, m) * abs(wi.dot(m)) * Eval(m) / std::abs(Frame::CosTheta(wi));
    }

    float MicrofacetDistribution::SmithG1(const Vector3f &v, const Vector3f &m) const {
        if (v.dot(m) * Frame::CosTheta(v) <= 0)
            return 0.0f;

        /* Perpendicular incidence -- no shadowing/masking */
        float tanTheta = std::abs(Frame::TanTheta(v));
        if (tanTheta == 0.0f)
            return 1.0f;

        float alpha = ProjectRoughness(v);
        float root = alpha * tanTheta;
        return 2.0f / (1.0f + hypot2((float) 1.0f, root));

    }

    float MicrofacetDistribution::ProjectRoughness(const Vector3f &v) const {
        float invSinTheta2 = 1.f / Frame::SinTheta2(v);

        if (invSinTheta2 <= 0)
            return alpha_;

        float cosPhi2 = v.x() * v.x() * invSinTheta2;
        float sinPhi2 = v.y() * v.y() * invSinTheta2;

        return std::sqrt(cosPhi2 * alpha_ * alpha_ + sinPhi2 * alpha_ * alpha_);
    }

    float MicrofacetDistribution::G(const Vector3f &wi, const Vector3f &wo, const Normal3f &m) const {
        return SmithG1(wi, m) * SmithG1(wo, m);
    }

    Normal3f MicrofacetDistribution::SampleVisible(const Vector3f &_wi, const Vector2f &sample) const {
        Vector3f wi = Vector3f(
                alpha_ * _wi.x(),
                alpha_ * _wi.y(),
                _wi.z()
        ).normalized();

        /* Get polar coordinates */
        float theta = 0, phi = 0;
        if (wi.z() < (float) 0.99999) {
            theta = std::acos(wi.z());
            phi = std::atan2(wi.y(), wi.x());
        }
        float sinPhi, cosPhi;
        sincos(phi, &sinPhi, &cosPhi);

        /* Step 2: simulate P22_{wi}(slope.x, slope.y, 1, 1) */
        Vector2f slope = Sample11(theta, sample);

        /* Step 3: rotate */
        slope = Vector2f(
                cosPhi * slope.x() - sinPhi * slope.y(),
                sinPhi * slope.x() + cosPhi * slope.y());

        /* Step 4: unstretch */
        slope.x() *= alpha_;
        slope.y() *= alpha_;

        /* Step 5: compute normal */
        float normalization = (float) 1 / std::sqrt(slope.x() * slope.x()
                                                    + slope.y() * slope.y() + (float) 1.0);

        return Normal3f(
                -slope.x() * normalization,
                -slope.y() * normalization,
                normalization
        ).normalized();
    }

}