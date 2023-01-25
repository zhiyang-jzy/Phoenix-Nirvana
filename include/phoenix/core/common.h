#pragma once

#include<Eigen/Eigen>
#include<limits>
#include<string>
#include<memory>

using std::string;
using std::shared_ptr;

typedef unsigned int uint;

typedef Eigen::Vector3f Vector3f;
typedef Eigen::Vector2f Vector2f;
typedef Eigen::Vector2i Vector2i;

constexpr float kEpsilon = 1e-4f;
constexpr float kFInf = std::numeric_limits<float>::infinity();
