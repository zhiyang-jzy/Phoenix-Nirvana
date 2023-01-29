#pragma once
#include "phoenix/core/common.h"
#include "phoenix/core/common_struct.h"
#include "phoenix/core/object_class.h"

namespace Phoenix {

    class Camera: public PhoenixObject{
    public:
        Vector2i output_size_;
        Vector2f inv_output_size_;
    public:
        Camera()=default;
        Camera(int width,int height):output_size_(width,height),inv_output_size_(1.f/width,1.f/height){}
        virtual void GenerateRay(Vector2f sample,Ray& ray)const =0;
        [[nodiscard]] PClassType GetClassType() const final{return PClassType::PCamera;}

    };
}