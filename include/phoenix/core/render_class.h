#include "common.h"
#include "scene_class.h"
#include "camera_class.h"
#include "sampler_class.h"
#include "integrator.h"
#include "bitmap_class.h"
#include "image_block_class.h"

namespace Phoenix {
    enum class ResType {
        Color, Normal, Albedo
    };

    class Renderer {

    public:

    private:
        shared_ptr<Scene> scene_;
        shared_ptr<Camera> camera_;
        shared_ptr<Sampler> sampler_;
        shared_ptr<Bitmap> bitmap_;
        shared_ptr<Integrator> integrator_;
        shared_ptr<Integrator> normal_integrator_,albedo_integrator_;
        shared_ptr<ImageBlock> result_, albedo_, normal_;
        Vector2i pic_size_;
        int sample_count_;
        bool multi_thread_;
    private:
        void RenderBlock(shared_ptr<Sampler> sampler, ImageBlock &block, shared_ptr<Integrator> integrator,
                         uint sample_count,
                         shared_ptr<ImageBlock> &res, bool random_offset);

    public:

        Renderer();

        void SetScene(shared_ptr<Scene> scene) { scene_ = std::move(scene); }

        void SetCamera(shared_ptr<Camera> camera) { camera_ = std::move(camera); }

        void SetSampler(shared_ptr<Sampler> sampler) { sampler_ = std::move(sampler); }

        void SetPicSize(Vector2i size) {
            pic_size_ = size;
            bitmap_ = make_shared<Bitmap>(size.x(), size.y());
        }

        void SetIntegrator(shared_ptr<Integrator> integrator) { integrator_ = std::move(integrator); }

        void SetSampleCount(int sample_count) { sample_count_ = sample_count; }

        shared_ptr<Bitmap> bitmap(ResType res_type);

        shared_ptr<Scene> scene() { return scene_; }

        void SetMultiThread(bool bIsOpen) { multi_thread_ = bIsOpen; }

    public:
        void Render();

    };
}