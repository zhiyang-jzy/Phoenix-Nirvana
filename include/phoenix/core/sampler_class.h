#pragma once

#include "phoenix/core/common.h"
#include "phoenix/core/object_class.h"

namespace Phoenix {
    class Sampler : public PhoenixObject {
    public:
        virtual float Next1D() = 0;

        virtual Vector2f Next2D() = 0;

        PClassType GetClassType() const final { return PClassType::PSampler; }

    };

    inline Vector3f SquareToUniformHemiSphere(const Vector2f &sample) {
        float z = sample.x();
        float tmp = sqrt(1.f - z * z);
        float sin_phi = sin(2.f * kPi * sample.y());
        float cos_phi = cos(2.f * kPi * sample.y());
        return {tmp * cos_phi, tmp * sin_phi, z};
    }

    inline float SquareToUniformHemiSpherePdf() { return kInvTowPi; }

    inline Vector3f SquareToUniformSphere(const Vector2f &sample) {
        float z = 1.0f - 2.0f * sample.y();
        float r = sqrt(1.0f - z * z);
        float t = 2.f * kPi * sample.x();
        float sin_phi = sin(t), cos_phi = cos(t);
        return {r * cos_phi, r * sin_phi, z};
    }

    inline Vector3f SquareToCosineHemisphere(const Vector2f &sample) {
        float theta = acos(2 * sample.x() - 1) / 2;
        float z = cos(theta);
        float r = sqrtf(1.f - z * z);
        float phi = 2 * kPi * sample.y();
        return {r * cos(phi), r * sin(phi), z};
    }
    inline float SquareToCosineHemispherePdf(const Vector3f &v) {
        return v.z() > 0 ? v.z() / kPi : 0.f;
    }



}