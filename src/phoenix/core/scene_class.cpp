#include "phoenix/core/scene_class.h"

#include <utility>
#include "phoenix/core/common.h"
#include "phoenix/core/shape_class.h"
#include "phoenix/core/emitter_class.h"

namespace Phoenix {

    void Scene::AddShape(const shared_ptr<Shape> &shape) {
        shapes_.push_back(shape);
        auto ids = shape->AddToTracer(tracer_);
        std::for_each(ids.begin(), ids.end(), [&](const auto &item) {
            shape_dict_[item] = shape;
        });
    }

    void Scene::SetDict(uint id, shared_ptr<Shape> shape) {
        shape_dict_[id] = std::move(shape);
    }

    Interaction Scene::Trace(const Ray &ray) const {
        TracerHit basic = tracer_.TraceRay(ray);
        basic.normal = basic.normal.normalized();
        Interaction interaction;
        interaction.basic = basic;
        interaction.shape = nullptr;
        interaction.emitter = nullptr;
        if (basic.is_hit) {
            if (shape_dict_.count(basic.geo_id)) {
                interaction.hit_type = HitType::Shape;
                interaction.shape = shape_dict_.at(basic.geo_id);
                interaction.frame = Frame(basic.normal);
                interaction.uv = interaction.shape->GetUv(interaction);

            } else {
                interaction.hit_type = HitType::Emitter;
                interaction.emitter = emitter_dict_.at(basic.geo_id);
                interaction.frame = Frame(basic.normal);
            }
        }
        return interaction;
    }

    void Scene::AddEmitter(const shared_ptr<Emitter> &emitter) {
        emitters_.push_back(emitter);
        auto area = emitter->area();
        emitter_dpdf_.Append(1.f);
        emitter->AddToScene(*this);
    }

    void Scene::SetDict(uint id, shared_ptr<Emitter> emitter) {
        emitter_dict_[id] = std::move(emitter);
    }

    shared_ptr<Emitter> Scene::SampleEmitter(float &pdf, float sample) const {
        if (emitters_.empty()) {
            pdf = 0.f;
            return nullptr;
        }
        auto index = emitter_dpdf_.Sample(sample, pdf);
        pdf = emitter_dpdf_.normalization();
        return emitters_[index];
    }

    Scene::Scene() {
        emitter_dpdf_.Clear();
    }

    void Scene::PostProcess() {
        FinishAdd();
        emitter_dpdf_.Normalize();
    }

    Color3f Scene::SampleEmitterDirect(DirectSamplingRecord &dRec, const Vector2f &sample_) const {
        float emPdf;
        Vector2f sample(sample_);
        size_t index = emitter_dpdf_.SampleReuse(sample.x(), emPdf);
        auto emitter = emitters_[index];
        Color3f value = emitter->SampleDirect(dRec, sample_);
        dRec.emitter = emitter;

        if (dRec.pdf != 0) {
            Ray ray(dRec.ref, dRec.dir);
            auto its = Trace(ray);
            if (its.basic.is_hit && its.hit_type == HitType::Emitter && its.basic.point.isApprox(dRec.point)) {
                dRec.pdf *= emPdf;
                value /= emPdf;
                return value;
            } else {
                return Color3f(0.0);
            }

        } else {
            return Color3f(0.0f);
        }
    }

    float Scene::PdfEmitterDiscrete(const DirectSamplingRecord &dRec) const {
        return emitter_dpdf_.normalization() * (dRec.dist * dRec.dist) / abs(dRec.normal.dot(dRec.dir));
    }
}