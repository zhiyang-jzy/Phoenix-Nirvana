#include "phoenix/core/integrator.h"
namespace Phoenix{
    class AlbedoIntegrator:public Integrator{
    public:
        AlbedoIntegrator(PropertyList properties)
        {

        }

        [[nodiscard]] string ToString()const override{
            return "normal";
        }

        [[nodiscard]] Color3f Li(shared_ptr<Scene> scene,shared_ptr<Sampler> sampler,const Ray& ray) const override
        {
            auto hit = scene->Trace(ray);
            if(!hit.basic.is_hit)
                return {0,0,0};
            if(hit.hit_type==HitType::Emitter)
                return hit.emitter->radiance();
            return hit.shape->base_color();

        }
    };

    PHOENIX_REGISTER_CLASS(AlbedoIntegrator,"albedo");
}