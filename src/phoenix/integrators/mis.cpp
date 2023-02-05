#include "phoenix/core/integrator.h"
#include "phoenix/core/scene_class.h"

namespace Phoenix {

    class Mis : public Integrator {
    private:
        float russian_ = 0.95;
        size_t mi_recur_ = 5;
    public:

        Mis(PropertyList properties) {

        }

        [[nodiscard]] string ToString() const override {
            return "base integrator";
        }

        [[nodiscard]] Color3f Li(shared_ptr<Scene> scene, shared_ptr<Sampler> sampler, const Ray &ray) const override {
            Color3f res(0, 0, 0);
            Color3f para(1.0f);
            size_t now_count = 0;
            float le_weight = 1.0, ne_weight = 0.0;
            auto now_ray = ray;
            while (now_count < mi_recur_ || sampler->Next1D() < russian_) {
                now_count++;
                auto hit = scene->Trace(now_ray);
                if (!hit.basic.is_hit) {
                    break;
                }

                if (hit.hit_type == HitType::Emitter) {
                    EmitterQueryRecord rec(now_ray.orig, hit.basic.point, hit.basic.normal);
//                    if (now_count == 1)
                    res += para.cwiseProduct(hit.emitter->Eval(rec)) * le_weight;
                    break;
                }

                auto bsdf = hit.shape->bsdf();
                BSDFQueryRecord basic_bsdf_rec(hit.frame.ToLocal(-now_ray.dir).normalized());

                float bsdf_pdf;
                bsdf->Sample(basic_bsdf_rec, bsdf_pdf, sampler->Next2D());

                {
                    EmitterQueryRecord emitter_rec(hit.basic.point);
                    float nee_pdf;
                    auto emitter = scene->SampleEmitter(nee_pdf, sampler->Next1D());

                    ne_weight = (nee_pdf) / (nee_pdf + bsdf_pdf);
                    le_weight = (bsdf_pdf) / (nee_pdf + bsdf_pdf);

                    auto emitter_color = emitter->Sample(emitter_rec, sampler->Next2D());
                    float cos_theta1 = (emitter_rec.wi).dot(emitter_rec.n);
                    float cos_theta = (-emitter_rec.wi).dot(hit.basic.normal);

                    BSDFQueryRecord bsdf_rec(hit.frame.ToLocal(-emitter_rec.wi), hit.frame.ToLocal(-now_ray.dir));
                    auto bsdf_v = bsdf->Eval(bsdf_rec);
                    auto shadow_hit = scene->Trace(emitter_rec.shadow_ray);
                    if (shadow_hit.basic.is_hit) {
                        if (shadow_hit.emitter == emitter && (shadow_hit.basic.point - emitter_rec.p).norm() <= kEpsilon) {
                            auto dis = (emitter_rec.ref - emitter_rec.p).squaredNorm();
                            res += emitter_color.cwiseProduct(bsdf_v).cwiseProduct(para) * cos_theta1 * cos_theta /
                                   nee_pdf / (dis) * ne_weight;
                        }
                    }


                }
                {


                    auto bsdf_v = bsdf->Eval(basic_bsdf_rec);
                    now_ray = Ray(hit.basic.point, hit.frame.ToWorld(basic_bsdf_rec.wo).normalized());

                    para = para.cwiseProduct(bsdf_v) * Frame::CosTheta(basic_bsdf_rec.wo) / bsdf_pdf;

                    if (now_count >= mi_recur_) {
                        para /= russian_;
                    }
                }
            }
            if (res.isValid())
                return res;
            return {0, 0, 0};


        }
    };

    PHOENIX_REGISTER_CLASS(Mis, "mis");

}