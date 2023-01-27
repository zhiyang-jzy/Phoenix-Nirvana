#pragma once

#include<Eigen/Eigen>
#include<limits>
#include<string>
#include<memory>
#include<vector>
#include<map>

using std::string;
using std::shared_ptr;
using std::map;
using std::vector;
using std::make_shared;

typedef unsigned int uint;

typedef Eigen::Vector3f Vector3f;
typedef Eigen::Vector2f Vector2f;
typedef Eigen::Vector2i Vector2i;
typedef Vector3f Color3f;

constexpr float kEpsilon = 1e-4f;
constexpr float kFInf = std::numeric_limits<float>::infinity();

const float kPi = acos(-1.f);
const float kTowPi = 2.f * kPi;
const float kInvPi = 1.f / kPi;
const float kInvTowPi = 1.f / (2.f * kPi);
const float kInvFourPi = 1.f / (4.f * kPi);

inline float Clamp(float value, float min, float max) {
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else return value;
}
