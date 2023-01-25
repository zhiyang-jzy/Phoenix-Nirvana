#include "phoenix/core/camera_class.h"

namespace Phoenix{
    class PinholeCamera:public Camera{
    public:
        PinholeCamera(PropertyList properties)
        {
            int width = properties.Get<int>("width").value_or(800);
            int height = properties.Get<int>("height").value_or(800);
            output_size_ = {width,height};
            inv_output_size_ = {1.f/width,1.f/height};
        }
        void GenerateRay(const Vector2f sample,Ray& ray)override{

            auto orig = Vector3f(0,0,0);
            Vector2f xy= (sample+Vector2f(0.5,0.5)).cwiseProduct(inv_output_size_);
            xy = xy*2-Vector2f(1.0,1.0);
            Vector3f dir(xy.x(),xy.y(),-1);
            dir.normalize();

            ray = {orig,dir,kEpsilon,kFInf};
        }
        [[nodiscard]] string ToString() const override{return "pinhole camera";}
    };

    PHOENIX_REGISTER_CLASS(PinholeCamera,"pinhole");
}