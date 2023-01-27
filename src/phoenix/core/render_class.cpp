#include "phoenix/core/render_class.h"

namespace Phoenix {

    void Renderer::Render() {
        int width = pic_size_.x(), height = pic_size_.y();
        Ray ray;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                {
                    Vector3f res(0,0,0);
                    for (int k = 0; k < sample_count_; k++) {
                        Vector2f sample = Vector2f(i,j)+sampler_->Next2D();
                        camera_->GenerateRay(sample, ray);
                        res += integrator_->Li(scene_, sampler_, ray);
                    }
                    res/= sample_count_;
                    bitmap_->coeffRef(i,j) = res;
                }
            }
        }
    }

    Renderer::Renderer() {
        scene_ = make_shared<Scene>();
    }
}