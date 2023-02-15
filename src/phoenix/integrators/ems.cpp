#include "phoenix/core/integrator.h"
#include "phoenix/core/scene_class.h"

namespace Phoenix {


    class Ems : public Integrator {
    private:
        float russian_ = 0.95;
    public:

        Ems(PropertyList properties) {

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
//                EmitterQueryRecord rec(ray.orig, hit.basic.point, hit.basic.normal);
//                return hit.emitter->Eval(rec);
                return {0,0,0};
            }
            auto bsdf = hit.shape->bsdf();


            //sample light
            {
                EmitterQueryRecord rec(hit.basic.point);
                float pdf;
                auto emitter = scene->SampleEmitter(pdf, sampler->Next1D());
                auto emitter_color = emitter->Sample(rec, sampler->Next2D());
                float cos_theta1 = (rec.wi).dot(rec.n);
                float cos_theta = (-rec.wi).dot(hit.basic.normal);

                BSDFQueryRecord bsdf_rec(hit.frame.ToLocal(-rec.wi), hit.frame.ToLocal(-ray.dir), Phoenix::Vector2f());
                auto bsdf_v = bsdf->Eval(bsdf_rec);
                auto shadow_hit = scene->Trace(rec.shadow_ray);
                if (shadow_hit.basic.is_hit) {
                    if (shadow_hit.emitter == emitter && (shadow_hit.basic.point - rec.p).norm() <= kEpsilon) {
                        auto dis = (rec.ref - rec.p).squaredNorm();
                        res += emitter_color.cwiseProduct(bsdf_v) * cos_theta1 * cos_theta / pdf / (dis);
                    }
                }

            }

            BSDFQueryRecord rec(hit.frame.ToLocal(-ray.dir).normalized(), Phoenix::Vector2f());

            float pdf;
            bsdf->Sample(rec, pdf, sampler->Next2D());
            auto bsdf_v = bsdf->Eval(rec);
            Ray light_ray(hit.basic.point, hit.frame.ToWorld(rec.wo).normalized());

            if (sampler->Next1D() < russian_)
                res += Li(scene, sampler, light_ray).cwiseProduct(bsdf_v) * Frame::CosTheta(rec.wo) / pdf / russian_;
            if (res.isValid())
                return res;
            return {0, 0, 0};
        }
    };

    PHOENIX_REGISTER_CLASS(Ems, "ems");

}