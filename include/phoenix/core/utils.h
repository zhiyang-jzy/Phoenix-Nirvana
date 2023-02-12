#pragma once

#include "common_struct.h"

namespace Phoenix {

    inline Vector3f Reflect(const Vector3f &wi) {
        return {-wi.x(), -wi.y(), wi.z()};
    }

    inline Vector3f Reflect(const Vector3f &wi, const Normal3f &m) {
        return 2 * wi.dot(m) * m - wi;
    }

    inline Color3f FresnelConductorExact(float cosThetaI, const Color3f &eta, const Color3f &k) {
        /* Modified from "Optics" by K.D. Moeller, University Science Books, 1988 */

        float cosThetaI2 = cosThetaI * cosThetaI,
                sinThetaI2 = 1 - cosThetaI2,
                sinThetaI4 = sinThetaI2 * sinThetaI2;

        Color3f temp1 = eta * eta - k * k - Color3f(sinThetaI2),
                a2pb2 = (temp1 * temp1 + k * k * eta * eta * 4).sqrt(),
                a = ((a2pb2 + temp1) * 0.5f).sqrt();

        Color3f term1 = a2pb2 + Color3f(cosThetaI2),
                term2 = a * (2 * cosThetaI);

        Color3f Rs2 = (term1 - term2) / (term1 + term2);

        Color3f term3 = a2pb2 * cosThetaI2 + Color3f(sinThetaI4),
                term4 = term2 * sinThetaI2;

        Color3f Rp2 = Rs2 * (term3 - term4) / (term3 + term4);

        return 0.5f * (Rp2 + Rs2);
    }

    inline float MiWeight(float pdf_a, float pdf_b) {
        pdf_a *= pdf_a;
        pdf_b *= pdf_b;
        return pdf_a / (pdf_b + pdf_a);
    }
    inline float hypot2(float a, float b) {
        float r;
        if (std::abs(a) > std::abs(b)) {
            r = b / a;
            r = std::abs(a) * std::sqrt(1.0f + r*r);
        } else if (b != 0.0f) {
            r = a / b;
            r = std::abs(b) * std::sqrt(1.0f + r*r);
        } else {
            r = 0.0f;
        }
        return r;
    }

    inline void sincos(float theta, float *_sin, float *_cos) {
        *_sin = sinf(theta);
        *_cos = cosf(theta);
    }
    inline float safe_sqrt(float value) {
        return std::sqrt(std::max(0.0f, value));
    }

}