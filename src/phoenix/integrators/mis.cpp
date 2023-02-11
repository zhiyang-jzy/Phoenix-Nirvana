#include "phoenix/core/integrator.h"
#include "phoenix/core/scene_class.h"

namespace Phoenix {

    class Mis : public Integrator {
    private:
        float russian_ = 0.95;
        size_t mi_recur_ = 10;
    public:

        Mis(PropertyList properties) {

        }

        [[nodiscard]] string ToString() const override {
            return "base integrator";
        }

        [[nodiscard]] Color3f Li(shared_ptr<Scene> scene, shared_ptr<Sampler> sampler, const Ray &ray) const override {
            Color3f res(0, 0, 0);
            Color3f throughput(1.0f);
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
                    if (now_count == 1)
                        res += throughput.cwiseProduct(hit.emitter->Eval(rec));
                    break;
                }

                auto bsdf = hit.shape->bsdf();

                auto b_is_specular = bsdf->IsSpecular();

                BSDFQueryRecord basic_bsdf_rec(hit.frame.ToLocal(-now_ray.dir).normalized());

                float bsdf_pdf;
                float emitter_pdf;
                bsdf->Sample(basic_bsdf_rec, bsdf_pdf, sampler->Next2D());
                if (!b_is_specular) {
                    EmitterQueryRecord emitter_rec(hit.basic.point);

                    auto emitter = scene->SampleEmitter(emitter_pdf, sampler->Next1D());
                    auto emitter_color = emitter->Sample(emitter_rec, sampler->Next2D());

                    BSDFQueryRecord nee_bsdf_rec(hit.frame.ToLocal(-now_ray.dir), hit.frame.ToLocal(-emitter_rec.wi));
                    float nee_bsdf_pdf = bsdf->Pdf(nee_bsdf_rec);

                    ne_weight = (emitter_pdf) / (emitter_pdf + nee_bsdf_pdf);


                    float cos_theta1 = (emitter_rec.wi).dot(emitter_rec.n);

                    //BSDFQueryRecord bsdf_rec(hit.frame.ToLocal(-emitter_rec.wi), hit.frame.ToLocal(-now_ray.dir));
                    BSDFQueryRecord bsdf_rec(hit.frame.ToLocal(-now_ray.dir), hit.frame.ToLocal(-emitter_rec.wi));
                    auto bsdf_v = bsdf->Eval(bsdf_rec);
                    auto shadow_hit = scene->Trace(emitter_rec.shadow_ray);
                    if (shadow_hit.basic.is_hit) {
                        if (shadow_hit.emitter == emitter &&
                            (shadow_hit.basic.point - emitter_rec.p).norm() <= kEpsilon) {
                            auto dis = (emitter_rec.ref - emitter_rec.p).squaredNorm();
                            res += emitter_color.cwiseProduct(bsdf_v).cwiseProduct(throughput) * cos_theta1 /
                                   emitter_pdf / (dis) * ne_weight;
                        }
                    }


                }
                {


                    auto bsdf_v = bsdf->Eval(basic_bsdf_rec);
                    throughput *= bsdf_v;
                    throughput /= bsdf_pdf;

                    now_ray = Ray(hit.basic.point, hit.frame.ToWorld(basic_bsdf_rec.wo).normalized());
                    bool hitEmitter = false;

                    auto temp_hit = scene->Trace(now_ray);

                    if (temp_hit.basic.is_hit) {
                        if (!b_is_specular) {
                            if (temp_hit.hit_type == HitType::Emitter) {
                                le_weight = bsdf_pdf / (scene->EmitterPdf() + bsdf_pdf);
                                EmitterQueryRecord rec(now_ray.orig, temp_hit.basic.point, temp_hit.basic.normal);
                                auto value = temp_hit.emitter->Eval(rec);
                                res += value * le_weight * throughput;
                                break;

                            } else {

                            }
                        } else {
                            if (temp_hit.hit_type == HitType::Emitter) {
                                EmitterQueryRecord rec(now_ray.orig, temp_hit.basic.point, temp_hit.basic.normal);
                                auto value = temp_hit.emitter->Eval(rec);
                                res += value * throughput;
                            }
                        }
                    } else {
                        break;
                    }


                    if (now_count >= mi_recur_) {
                        throughput /= russian_;
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