#include "phoenix/core/shape_class.h"
#include "phoenix/core/scene_class.h"

namespace Phoenix {

    class Sphere : public Shape {
    private:
        Vector3f center_;
        float radius_;
    public:
        explicit Sphere(PropertyList properties) {
            center_ = properties.Get<vector<float>>("center").value_or(vector<float>{0, 0, 0});
            radius_ = properties.Get<double>("radius").value_or(1.);
            area_ = 4.f * kPi * radius_ * radius_;
        }

        [[nodiscard]] string ToString() const override { return "sphere"; }

        vector<uint> AddToTracer(Tracer &tracer) override {
            uint id = tracer.AddSphere(center_, radius_);
            return {id};
        }

        void SamplePosition(PositionSampleRecord &rec, Vector2f sample) override {
            auto v = SquareToUniformSphere(sample).normalized();
            rec.point = center_ + radius_ * v;
            rec.normal = v;
            rec.pdf = 1.f / area_;
        }

        void ApplyTransform(const Transform &trans) override {

        }

        Vector2f GetUv(const Interaction &its) const override {
            Vector3f n = (its.basic.point - center_).normalized();
            float u = atan2(n.x(), n.y()) / (kTowPi) + 0.5;
            float v = n.y() * 0.5 + 0.5;
            return {u, v};

        }


    };

    PHOENIX_REGISTER_CLASS(Sphere, "sphere");

}