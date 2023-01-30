#include "phoenix/core/camera_class.h"
#include "phoenix/utils/tool_function.h"
#include <spdlog/spdlog.h>

namespace Phoenix {
    class PinholeCamera : public Camera {
    private:
        float aspect_, scale_;
        float fov_;
    public:
        explicit PinholeCamera(PropertyList properties) {
            output_size_.x() = properties.Get<long long>("width").value_or(800);
            output_size_.y() = properties.Get<long long>("height").value_or(800);
            fov_ = DegToRad(properties.Get<double>("fov").value_or(45.0));
            scale_ = tan(fov_ * 0.5f);
            aspect_ = output_size_.x() / float(output_size_.y());
            inv_output_size_ = {1.f / output_size_.x(), 1.f / output_size_.y()};


            orig_ = properties.Get<Vector3f>("orig").value();
            target_ = properties.Get<Vector3f>("target").value();
            up_ = properties.Get<Vector3f>("up").value();

            look_at_ = CalcLookAt(orig_, target_, up_);


        }

        void GenerateRay(const Vector2f sample, Ray &ray) const override {

            auto orig = look_at_.inverse() * Point3f(0, 0, 0);

            float x = (sample.x() * inv_output_size_.x() - 0.5f) * 2.f * aspect_ * scale_;
            float y = (sample.y() * inv_output_size_.y() - 0.5f) * 2.f * scale_;

            Vector3f dir(x, y, -1);
            dir = look_at_.inverse() * dir;
            dir.normalize();

            ray = Ray(orig, dir);
        }

        [[nodiscard]] string ToString() const override { return "pinhole camera"; }
    };

    PHOENIX_REGISTER_CLASS(PinholeCamera, "pinhole");
}