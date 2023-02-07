#include "phoenix/core/common_struct.h"

namespace Phoenix {


    Ray::Ray(Vector3f ori, Vector3f di) : orig(std::move(ori)), dir(std::move(di)) { t_near = 1e-4, t_far = kFInf; }

    Vector3f Ray::At(float t) const {
        return orig + dir * t;
    }
};