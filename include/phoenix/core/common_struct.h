#pragma once

#include "common.h"
namespace Phoenix{
    struct Ray{
        Vector3f orig;
        Vector3f dir;
        float t_near;
        float t_far;
    };

    struct TracerHit{
        bool is_hit;
        float t_far;
        Vector2f uv;
        Vector3f normal;
        uint geo_id;
        uint prim_id;
    };
}