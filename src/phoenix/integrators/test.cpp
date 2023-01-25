#include "phoenix/core/integrator.h"
namespace Phoenix{

    class TestIntegrator : public Integrator{

    public:

        TestIntegrator(PropertyList properties)
        {

        }

        [[nodiscard]] string ToString()const override{
            return "test integrator";
        }

        Vector3f Li(shared_ptr<Scene> scene,shared_ptr<Sampler> sampler,const Ray& ray) override
        {
            return {0,1,0};
        }
    };

    PHOENIX_REGISTER_CLASS(TestIntegrator,"test");

}