#include "phoenix/core/render_class.h"
namespace Phoenix{

    void Renderer::Render() {
        int width = pic_size_.x(),height = pic_size_.y();
        Ray ray;
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++){
                camera_->GenerateRay({i,j},ray);
                bitmap_->coeffRef(i,j) =integrator_->Li(scene_,sampler_,ray);
            }
        }
    }
}