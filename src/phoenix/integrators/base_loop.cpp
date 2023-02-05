#include "phoenix/core/integrator.h"
#include "phoenix/core/scene_class.h"

namespace Phoenix {

    class BaseLoop : public Integrator {
    private:
        float russian_ = 0.95;
        size_t mi_recur_ = 5;
    public:

        BaseLoop(PropertyList properties) {

        }

        [[nodiscard]] string ToString() const override {
            return "base integrator";
        }

        [[nodiscard]] Color3f Li(shared_ptr<Scene> scene, shared_ptr<Sampler> sampler, const Ray &ray) const override {
            Color3f res(0, 0, 0);
            Color3f para(1.0f);
            size_t now_count = 0;
            auto now_ray = ray;
            while (now_count < mi_recur_ || sampler->Next1D() < russian_) {
                now_count++;
                auto hit = scene->Trace(now_ray);
                if (!hit.basic.is_hit) {
                    break;
                }

                if (hit.hit_type == HitType::Emitter) {
                    EmitterQueryRecord rec(now_ray.orig, hit.basic.point, hit.basic.normal);
                    if(now_count==1)
                        res += para.cwiseProduct(hit.emitter->Eval(rec));
                    break;
                }

                auto bsdf = hit.shape->bsdf();

                {
                    EmitterQueryRecord rec(hit.basic.point);
                    float pdf;
                    auto emitter = scene->SampleEmitter(pdf, sampler->Next1D());
                    auto emitter_color = emitter->Sample(rec, sampler->Next2D());
                    float cos_theta1 = (rec.wi).dot(rec.n);
                    float cos_theta = (-rec.wi).dot(hit.basic.normal);

                    BSDFQueryRecord bsdf_rec(hit.frame.ToLocal(-rec.wi), hit.frame.ToLocal(-now_ray.dir));
                    auto bsdf_v = bsdf->Eval(bsdf_rec);
                    auto shadow_hit = scene->Trace(rec.shadow_ray);
                    if (shadow_hit.basic.is_hit) {
                        if (shadow_hit.emitter == emitter && (shadow_hit.basic.point - rec.p).norm() <= kEpsilon) {
                            auto dis = (rec.ref - rec.p).squaredNorm();
                            res += emitter_color.cwiseProduct(bsdf_v).cwiseProduct(para) * cos_theta1 * cos_theta / pdf / (dis);
                        }
                    }


                }
                {

                    BSDFQueryRecord rec(hit.frame.ToLocal(-now_ray.dir).normalized());

                    float pdf;
                    bsdf->Sample(rec, pdf, sampler->Next2D());
                    auto bsdf_v = bsdf->Eval(rec);
                    now_ray = Ray(hit.basic.point, hit.frame.ToWorld(rec.wo).normalized());

                    para = para.cwiseProduct(bsdf_v) * Frame::CosTheta(rec.wo) / pdf;

                    if (now_count >= mi_recur_) {
                        para /= russian_;
                    }
                }
            }
            if(res.isValid())
                return res;
            return {0,0,0};


        }
    };

    PHOENIX_REGISTER_CLASS(BaseLoop, "base_loop");

}