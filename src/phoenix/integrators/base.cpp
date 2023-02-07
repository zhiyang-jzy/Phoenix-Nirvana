#include "phoenix/core/integrator.h"
#include "phoenix/core/scene_class.h"

namespace Phoenix {

    class Base : public Integrator {
    private:
        float russian_ = 0.99;
    public:

        Base(PropertyList properties) {

        }

        [[nodiscard]] string ToString() const override {
            return "base integrator";
        }

        Color3f Li(shared_ptr<Scene> scene, shared_ptr<Sampler> sampler, const Ray &ray) const override {
            auto hit = scene->Trace(ray);
            Color3f res(0, 0, 0);
            if (!hit.basic.is_hit)
                return {0, 0, 0};
            if (hit.hit_type == HitType::Emitter) {
                EmitterQueryRecord rec(ray.orig, hit.basic.point, hit.basic.normal);
                return hit.emitter->Eval(rec);
            }
            auto bsdf = hit.shape->bsdf();

            BSDFQueryRecord rec(hit.frame.ToLocal(-ray.dir).normalized());

            float pdf;
            bsdf->Sample(rec, pdf, sampler->Next2D());
            auto bsdf_v = bsdf->Eval(rec);
            Ray light_ray(hit.basic.point, hit.frame.ToWorld(rec.wo).normalized());

            if (sampler->Next1D() < russian_)
                res += Li(scene, sampler, light_ray).cwiseProduct(bsdf_v)/ pdf / russian_;
            if (res.isValid())
                return res;
            return {0, 0, 0};
        }
    };

    PHOENIX_REGISTER_CLASS(Base, "base");

}