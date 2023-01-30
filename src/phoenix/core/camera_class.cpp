#include "phoenix/core/camera_class.h"

namespace Phoenix {
    Transform Camera::CalcLookAt(Vector3f p, Vector3f target, Vector3f up) {

//        Vector3f dir = (p - target).normalized();
//        Vector3f left = dir.cross(up).normalized();
//        Vector3f newUp = left.cross(dir).normalized();
        Vector3f dir = (p - target).normalized();
        Vector3f left = up.cross(dir).normalized();
        Vector3f newUp = dir.cross(left).normalized();


        Eigen::Matrix4f result;
        result.coeffRef(0, 0) = left.x();
        result.coeffRef(1, 0) = left.y();
        result.coeffRef(2, 0) = left.z();
        result.coeffRef(3, 0) = 0;
        result.coeffRef(0, 1) = newUp.x();
        result.coeffRef(1, 1) = newUp.y();
        result.coeffRef(2, 1) = newUp.z();
        result.coeffRef(3, 1) = 0;
        result.coeffRef(0, 2) = dir.x();
        result.coeffRef(1, 2) = dir.y();
        result.coeffRef(2, 2) = dir.z();
        result.coeffRef(3, 2) = 0;
        result.coeffRef(0, 3) = p.x();
        result.coeffRef(1, 3) = p.y();
        result.coeffRef(2, 3) = p.z();
        result.coeffRef(3, 3) = 1;
        return Transform(result.inverse());
    }
}