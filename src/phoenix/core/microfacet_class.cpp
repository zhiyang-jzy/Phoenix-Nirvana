#include "phoenix/core/microfacet_class.h"

namespace Phoenix {

    float MicrofacetDistribution::Eval(const Vector3f &m) {
        if (Frame::CosTheta(m) <= 0)
            return 0.f;
        float cos_theta2 = Frame::cosTheta2(m);
        float sin_theta2 = Frame::SinTheta2(m);
        float beckmann = (sin_theta2 / cos_theta2) / (alpha_ * alpha_);
        float root = (1.f + beckmann) * cos_theta2;
        float res = 1.f / (kPi * alpha_ * alpha_ * root * root);
        if (res * Frame::CosTheta(m) < 1e-20f)
            res = 0;
        return res;
    }

    Normal3f MicrofacetDistribution::Sample(const Vector3f &wi_, const Vector2f &sample, float &pdf) {

        Vector3f wi = Vector3f(alpha_ * wi.x(), alpha_ * wi.y(), wi.z()).normalized();
        float theta = 0, phi = 0;
        if (wi.z() < 0.99999f) {
            theta = acos(wi.z());
            phi = atan2(wi.y(), wi.x());
        }
        float sin_phi, cos_phi;
        sin_phi = sin(phi);
        cos_phi = cos(phi);

        Vector2f slope = Sample11(theta, sample);
        slope = Vector2f(cos_phi * slope.x() - sin_phi * slope.y(), sin_phi * slope.x() + cos_phi * slope.y());
        slope.x() *= alpha_;
        slope.y() *= alpha_;

        float norm = 1.f / sqrt(slope.x() * slope.x()
                                + slope.y() * slope.y() + 1.f);

        auto m = Normal3f(-slope.x() * norm, -slope.y() * norm, norm).normalized();
        pdf = Pdf(wi_, m);
        return m;


    }

    inline float sig_num(float x) {
        return (x > 0) - (x < 0);
    }

    Vector2f MicrofacetDistribution::Sample11(float theta, Vector2f sample) {
        const float sqrt_pi_inv = 1.f / sqrt(kPi);
        Vector2f slope;
        if (theta < 1e-4f) {
            float sin_phi, cos_phi;
            float r = sqrt(sample.x() / (1 - sample.x()));
            sin_phi = sin(2 * kPi * sample.y());
            cos_phi = cos(2 * kPi * sample.y());
            return {r * cos_phi, r * sin_phi};
        }

        float tan_theta_i = tan(theta);
        float a = 1.f / tan_theta_i;
        float G1 = 2.f / (1.f + sqrt(1.f + 1.f / (a * a)));

        float A = 2.f * sample.x() / G1 - 1.f;
        if (abs(A) == 1)
            A -= sig_num(A) * kEpsilon;
        float tmp = 1.f / (A * A - 1.f);
        float B = tan_theta_i;
        float D = sqrt(B * B * tmp * tmp - (A * A - B * B) * tmp);
        float slope_x_1 = B * tmp - D;
        float slope_x_2 = B * tmp + D;
        slope.x() = (A < 0.0f || slope_x_2 > 1.0f / tan_theta_i) ? slope_x_1 : slope_x_2;

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
        return SmithG1(wi, m) * abs(wi.dot(m)) * Eval(m) /  abs(Frame::CosTheta(wi));
    }

    float MicrofacetDistribution::SmithG1(const Vector3f &v, const Vector3f &m) const {
        if (v.dot(m) * Frame::CosTheta(v) <= 0)
            return 0.f;
        float tan_theta = abs(Frame::TanTheta(v));
        if (tan_theta == 0.f)
            return 1.f;
        float alpha = ProjectRoughness(v);
        float root = alpha * tan_theta;
        return 2.0f / (1.0f + sqrt(1 + root * root));

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

}