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
