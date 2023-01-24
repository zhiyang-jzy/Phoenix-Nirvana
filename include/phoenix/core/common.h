#pragma once

#include<Eigen/Eigen>
#include<limits>

typedef unsigned int uint;

typedef Eigen::Vector3f Vector3f;

constexpr float kEpsilon = 1e-4f;
constexpr float kFInf = std::numeric_limits<float>::infinity();
