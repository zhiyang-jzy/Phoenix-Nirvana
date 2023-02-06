#include "phoenix/core/render_class.h"
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <tbb/task_arena.h>
#include <chrono>
#include "ext/indicators.hpp"

using namespace indicators;

namespace Phoenix {
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<double> duration;

    void Renderer::Render() {
        int width = pic_size_.x(), height = pic_size_.y();
        result_ = make_shared<ImageBlock>(pic_size_);
        albedo_ = make_shared<ImageBlock>(pic_size_);
        normal_ = make_shared<ImageBlock>(pic_size_);

        auto current_time = clock::now();
        spdlog::info("render start");
        BlockGenerator blockGenerator(pic_size_);
        result_->Clear();
        tbb::task_arena ta(multi_thread_ ? -1 : 1);
        ta.execute([&] {
            //tbb::task_scheduler_init init(thread_count_);
            tbb::blocked_range<int> range(0, blockGenerator.GetBlockCount());
            auto map = [&](const tbb::blocked_range<int> &range) {
                ImageBlock block(Vector2i(BlockGenerator::kBlockSize, BlockGenerator::kBlockSize));
                std::shared_ptr<Sampler> sampler(sampler_->Clone());
                for (int i = range.begin(); i < range.end(); ++i) {
                    blockGenerator.Next(block);
                    sampler->Prepare(block);
                    RenderBlock(sampler, block, integrator_, sample_count_, result_);
                    RenderBlock(sampler, block, normal_integrator_, 1, normal_);
                    RenderBlock(sampler, block, albedo_integrator_, 1, albedo_);
                }
            };
            tbb::parallel_for(range, map);
        });
        auto end_time = clock::now();
        duration use_time = end_time - current_time;
        spdlog::info("use {} ", use_time.count());

    }

    Renderer::Renderer() {
        scene_ = make_shared<Scene>();
        multi_thread_ = false;
        {
            PropertyList proper;
            normal_integrator_ = std::dynamic_pointer_cast<Integrator>(
                    PhoenixObjectFactory::CreateInstance("normal", proper));
            albedo_integrator_ = std::dynamic_pointer_cast<Integrator>(
                    PhoenixObjectFactory::CreateInstance("albedo", proper));
        }

    }

    void Renderer::RenderBlock(shared_ptr<Sampler> sampler, ImageBlock &block, shared_ptr<Integrator> integrator,
                               uint sample_count,
                               shared_ptr<ImageBlock> &res) {
        Vector2i offset = block.offset();
        Vector2i size = block.GetSize();
        block.Clear();

        for (int y = 0; y < size.y(); ++y) {
            for (int x = 0; x < size.x(); ++x) {
                Color3f res_color(0, 0, 0);
                for (uint32_t i = 0; i < sample_count; ++i) {
                    Vector2f pixelSample =
                            Vector2f((float) (x + offset.x()), (float) (y + offset.y())) + sampler->Next2D();
                    Ray ray;
                    camera_->GenerateRay(pixelSample, ray);
                    Color3f value = integrator->Li(scene_, sampler, ray);
                    res_color += value;
                }
                res_color /= sample_count;
                block.Put(Vector2f(x + offset.x(), y + offset.y()), res_color);
            }
        }
        res->Put(block);

    }

    shared_ptr<Bitmap> Renderer::bitmap(ResType res_type) {
        if (res_type == ResType::Color)
            return result_->ToBitmap();
        if (res_type == ResType::Albedo)
            return albedo_->ToBitmap();
        if (res_type == ResType::Normal)
            return normal_->ToBitmap();
    }
}
