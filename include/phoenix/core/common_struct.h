#pragma once

#include "common.h"
namespace Phoenix{
    struct Ray{
        Vector3f orig;
        Vector3f dir;
        float t_near;
        float t_far;
    };
}