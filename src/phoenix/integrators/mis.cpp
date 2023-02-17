#include "phoenix/core/integrator.h"
#include "phoenix/core/scene_class.h"
#include "phoenix/core/utils.h"

namespace Phoenix {

    class Mis : public Integrator {
    private:
        float russian_ = 0.95;
        size_t mi_recur_ = 10;
        size_t m_maxDepth = 50;
    public:

        Mis(PropertyList properties) {

        }

        [[nodiscard]] string ToString() const override {
            return "base integrator";
        }

        [[nodiscard]] Color3f Li(shared_ptr<Scene> scene, shared_ptr<Sampler> sampler, const Ray &_ray) const override {

            Interaction its;
            its = scene->Trace(_ray);
            Ray ray(_ray);
            Color3f Li(0.0f);
            Color3f throughput(1.0f);

            float temp = 0.f;
            size_t depth = 0;
            size_t m = 1;

            float wi1 = 0.f, wi2 = 0.f;

            while (true) {
                if (!its.basic.is_hit) {
                    break;
                }
                depth += 1;

                if (its.hit_type == HitType::Emitter) {

                    EmitterQueryRecord erec(ray.orig, its.basic.point, its.normal);
                    Li += throughput * its.emitter->Eval(erec);
                    break;
                }

                auto bsdf = its.shape->bsdf();


                DirectSamplingRecord dRec(its.basic.point);

                if (!bsdf->IsSpecular()) {
                    auto value = scene->SampleEmitterDirect(dRec, sampler->Next2D());
                    if (!value.isZero()) {
                        auto emitter = dRec.emitter;
                        BSDFQueryRecord bRec(its.frame.ToLocal(-ray.dir).normalized(),
                                             its.frame.ToLocal(dRec.dir).normalized(), its.uv);
                        auto bsdf_val = bsdf->Eval(bRec);
                        if (!bsdf_val.isZero()) {
                            float bsdf_pdf = bsdf->Pdf(bRec);
                            float weight = MiWeight(dRec.pdf, bsdf_pdf);
                            temp += weight;
                            wi1 = weight;
                            Li += throughput * value * bsdf_val * weight;
                        }

                    }
                }


                float bsdfPdf;
                BSDFQueryRecord bRec(its.frame.ToLocal(-ray.dir).normalized(), its.uv);
                Color3f bsdfWeight = bsdf->Sample(bRec, bsdfPdf, sampler->Next2D());
                if (bsdfWeight.isZero())
                    break;

                const Vector3f wo = its.frame.ToWorld(bRec.wo).normalized();

                bool hitEmitter = false;
                Color3f value(0.f);

                ray = Ray(its.basic.point, wo);
                its = scene->Trace(ray);
                if (its.basic.is_hit) {
                    if (its.hit_type == HitType::Emitter) {
                        EmitterQueryRecord erec(ray.orig, its.basic.point, its.normal);
                        value = its.emitter->Eval(erec);
                        dRec.SetQuery(ray, its);
                        hitEmitter = true;
                    }
                } else {
                }

                throughput *= bsdfWeight;

                if (hitEmitter) {
                    float emitter_pdf = scene->PdfEmitterDirect(dRec);

                    const float lumPdf = bsdf->IsSpecular() ?
                                         0 : emitter_pdf;
                    wi2 = MiWeight(bsdfPdf, lumPdf);
                    temp += MiWeight(bsdfPdf, lumPdf);
                    Li += throughput * value * MiWeight(bsdfPdf, lumPdf);
                    break;
                }

                if (!its.basic.is_hit)
                    break;

                if (depth >= mi_recur_) {

                    if (sampler->Next1D() > russian_)
                        break;
                    throughput /= russian_;
                }
            }

            if (Li.isValid())
                return Li;
            return {0, 0, 0};

//            if (temp < kEpsilon || abs(temp - 1) < kEpsilon)
//                return {0, 0, 0};
            return {temp, wi1, wi2};


        }
    };

    PHOENIX_REGISTER_CLASS(Mis, "mis");

}