#include "phoenix/core/integrator.h"
#include "phoenix/core/scene_class.h"
namespace Phoenix{

    class TestIntegrator : public Integrator{

    public:

        TestIntegrator(PropertyList properties)
        {

        }

        [[nodiscard]] string ToString()const override{
            return "test integrator";
        }

        [[nodiscard]] Color3f Li(shared_ptr<Scene> scene,shared_ptr<Sampler> sampler,const Ray& ray) const override
        {
            auto hit = scene->Trace(ray);
            if(!hit.basic.is_hit)
                return {0,0,0};
            return {1,1,1};
            //return hit.basic.normal;
        }
    };

    PHOENIX_REGISTER_CLASS(TestIntegrator,"test");

}