#include "phoenix/core/integrator.h"
namespace Phoenix{
    class NormalIntegrator:public Integrator{
    public:
        NormalIntegrator(PropertyList properties)
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
            auto normal = hit.basic.normal;
            normal.normalize();
            return {normal.x(),normal.y(),normal.z()};
        }
    };

    PHOENIX_REGISTER_CLASS(NormalIntegrator,"normal");
}