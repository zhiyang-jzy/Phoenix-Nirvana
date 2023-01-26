#include "phoenix/core/shape_class.h"
#include "phoenix/core/scene_class.h"
namespace Phoenix{

    class Sphere : public Shape{
    private:
        Vector3f center_;
        float radius_;
    public:
        Sphere(PropertyList properties){
            center_ = properties.Get<Vector3f>("center").value_or(Vector3f(0,0,0));
            radius_ = properties.Get<float>("radius").value_or(1.f);
            base_color_ = properties.Get<Vector3f>("color").value_or(Vector3f(1,1,1));
        }
        [[nodiscard]] string ToString()const override{return "sphere";}
        void AddToScene(Scene &scene)override
        {
            uint id = scene.tracer().AddSphere(center_,radius_);
            scene.SetDict(id, shared_ptr<Shape>(this));
        }

        Vector3f GetColor(const TracerHit& hit)override{
            return base_color_;
        }

    };

    PHOENIX_REGISTER_CLASS(Sphere,"sphere");

}