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
            r = std::abs(a) * std::sqrt(1.0f + r * r);
        } else if (b != 0.0f) {
            r = a / b;
            r = std::abs(b) * std::sqrt(1.0f + r * r);
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

    inline float fresnelDiffuseReflectance(float eta) {
        /* Fast mode: the following code approximates the
         * diffuse Frensel reflectance for the eta<1 and
         * eta>1 cases. An evalution of the accuracy led
         * to the following scheme, which cherry-picks
         * fits from two papers where they are best.
         */
        if (eta < 1) {
            /* Fit by Egan and Hilgeman (1973). Works
               reasonably well for "normal" IOR values (<2).

               Max rel. error in 1.0 - 1.5 : 0.1%
               Max rel. error in 1.5 - 2   : 0.6%
               Max rel. error in 2.0 - 5   : 9.5%
            */
            return -1.4399f * (eta * eta)
                   + 0.7099f * eta
                   + 0.6681f
                   + 0.0636f / eta;
        } else {
            /* Fit by d'Eon and Irving (2011)
             *
             * Maintains a good accuracy even for
             * unrealistic IOR values.
             *
             * Max rel. error in 1.0 - 2.0   : 0.1%
             * Max rel. error in 2.0 - 10.0  : 0.2%
             */
            float invEta = 1.0f / eta,
                    invEta2 = invEta * invEta,
                    invEta3 = invEta2 * invEta,
                    invEta4 = invEta3 * invEta,
                    invEta5 = invEta4 * invEta;

            return 0.919317f - 3.4793f * invEta
                   + 6.75335f * invEta2
                   - 7.80989f * invEta3
                   + 4.98554f * invEta4
                   - 1.36881f * invEta5;
        }

    }

}