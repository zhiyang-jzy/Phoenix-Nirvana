#include <utility>

#include "common.h"
#include "scene_class.h"
#include "camera_class.h"
#include "sampler_class.h"
#include "integrator.h"
#include "bitmap_class.h"

namespace Phoenix{
    class Renderer{
    private:
        shared_ptr<Scene> scene_;
        shared_ptr<Camera> camera_;
        shared_ptr<Sampler> sampler_;
        shared_ptr<Bitmap> bitmap_;
        shared_ptr<Integrator> integrator_;
        Vector2i pic_size_;
    public:

        Renderer()=default;
        void SetScene(shared_ptr<Scene> scene){scene_=std::move(scene);}
        void SetCamera(shared_ptr<Camera> camera){camera_=std::move(camera);}
        void SetSampler(shared_ptr<Sampler> sampler){sampler_=std::move(sampler);}
        void SetPicSize(Vector2i size){pic_size_=size;bitmap_= make_shared<Bitmap>(size.x(),size.y());}
        void SetIntegrator(shared_ptr<Integrator> integrator){integrator_=std::move(integrator);}
        shared_ptr<Bitmap> bitmap(){return bitmap_;}

    public:
        void Render();

    };
}