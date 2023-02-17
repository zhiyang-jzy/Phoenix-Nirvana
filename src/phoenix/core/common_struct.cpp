#include "phoenix/core/common_struct.h"

namespace Phoenix {


    Ray::Ray(Vector3f ori, Vector3f di) : orig(std::move(ori)), dir(std::move(di.normalized())) { t_near = 1e-3, t_far = kFInf; }

    Vector3f Ray::At(float t) const {
        return orig + dir * t;
    }

    Transform::Transform(vector<float> &vector) {
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                m_inverse.coeffRef(i,j) = vector[i*4+j];
            }
        }
        //m_transform =  Eigen::Map<Eigen::Matrix4f>(vector.data());
        m_transform = m_inverse.inverse();
    }
};