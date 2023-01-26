#include "phoenix/core/scene_class.h"
#include "phoenix/core/common.h"
#include "phoenix/core/shape_class.h"
namespace Phoenix {

    void Scene::AddShape(const shared_ptr<Shape>& shape) {
        shapes_.push_back(shape);
        shape->AddToScene(*this);

    }

    void Scene::SetDict(uint id, shared_ptr<Shape> shape) {
        shape_dict_[id] = std::move(shape);
    }

    Interaction Scene::Trace(const Ray& ray) {
        TracerHit basic = tracer_.TraceRay(ray);
        shared_ptr<Shape> shape = nullptr;
        if(basic.is_hit)
            shape = shape_dict_[basic.geo_id];
        return {basic,shape};
    }
}