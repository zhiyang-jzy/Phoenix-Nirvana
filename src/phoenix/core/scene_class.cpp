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

    Interaction Scene::Trace(const Ray &ray)const  {
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

            } else {
                interaction.hit_type = HitType::Emitter;
                interaction.emitter = emitter_dict_.at(basic.geo_id);
                interaction.frame = Frame(basic.normal);
            }
        }
        return interaction;
    }

    void Scene::AddEmitter(const shared_ptr<Emitter>& emitter) {
        emitters_.push_back(emitter);
        auto area = emitter->area();
        dpdf_.Append(area);
        emitter->AddToScene(*this);
    }

    void Scene::SetDict(uint id, shared_ptr<Emitter> emitter) {
        emitter_dict_[id] = std::move(emitter);
    }

    shared_ptr<Emitter> Scene::SampleEmitter(float &pdf, float sample) const {
        if(emitters_.empty())
        {
            pdf=0.f;
            return nullptr;
        }
        auto index = dpdf_.Sample(sample,pdf);
        pdf = dpdf_.normalization();
        return emitters_[index];
    }

    Scene::Scene() {
        dpdf_.Clear();
    }

    void Scene::PostProcess() {
        dpdf_.Normalize();
    }
}