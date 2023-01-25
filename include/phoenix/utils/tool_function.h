#pragma once

#include "phoenix/core/common.h"

namespace Phoenix {
    inline float DegToRad(float deg) {
        return deg * kPi / 180.f;
    }
}