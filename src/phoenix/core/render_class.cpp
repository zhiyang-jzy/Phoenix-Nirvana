#include "phoenix/core/render_class.h"
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <chrono>
#include "ext/indicators.hpp"
using namespace indicators;

namespace Phoenix {
    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::duration<double> duration;

    void Renderer::Render() {
        int width = pic_size_.x(), height = pic_size_.y();

        auto current_time = clock::now();
        spdlog::info("render start");
        if (!multi_thread_) {
            Ray ray;
            indicators::ProgressBar bar{
                    option::BarWidth{50},
                    option::Start{" ["},
                    option::Fill{"="},
                    option::Lead{"="},
                    option::Remainder{"-"},
                    option::End{"]"},
                    option::PrefixText{"Rendering "},
                    option::ForegroundColor{Color::yellow},
                    option::ShowElapsedTime{true},
                    option::ShowRemainingTime{true},
                    option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
            };

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    {
                        Color3f res(0, 0, 0);
                        for (int k = 0; k < sample_count_; k++) {
                            Vector2f sample = Vector2f(i, j) + sampler_->Next2D();
                            camera_->GenerateRay(sample, ray);
                            res += integrator_->Li(scene_, sampler_, ray);
                        }
                        bar.set_progress((i*width+j)*100/(height*width));
                        res /= sample_count_;
                        bitmap_->SetColor(i,j,res);
                    }
                }
            }
        } else {
            BlockGenerator blockGenerator(pic_size_);
            result_ = make_shared<ImageBlock>(pic_size_);
            result_->Clear();
            std::thread render_thread([&] {
                //tbb::task_scheduler_init init(thread_count_);
                tbb::blocked_range<int> range(0, blockGenerator.GetBlockCount());
                auto map = [&](const tbb::blocked_range<int> &range) {
                    ImageBlock block(Vector2i(BlockGenerator::kBlockSize, BlockGenerator::kBlockSize));
                    std::shared_ptr<Sampler> sampler(sampler_->Clone());
                    for (int i = range.begin(); i < range.end(); ++i) {
                        blockGenerator.Next(block);
                        sampler->Prepare(block);
                        RenderBlock(sampler, block);
                        result_->Put(block);
                    }
                };
                tbb::parallel_for(range, map);
            });
            render_thread.join();
            bitmap_ = result_->ToBitmap();
        }
        auto end_time = clock::now();
        duration use_time = end_time - current_time;
        spdlog::info("use {} ", use_time.count());

    }

    Renderer::Renderer() {
        scene_ = make_shared<Scene>();
        multi_thread_ = false;
    }

    void Renderer::RenderBlock(shared_ptr<Sampler> sampler, ImageBlock &block) {
        Vector2i offset = block.offset();
        Vector2i size = block.GetSize();
        block.Clear();

        for (int y = 0; y < size.y(); ++y) {
            for (int x = 0; x < size.x(); ++x) {
                Color3f res(0, 0, 0);
                for (uint32_t i = 0; i < sample_count_; ++i) {
                    Vector2f pixelSample =
                            Vector2f((float) (x + offset.x()), (float) (y + offset.y())) + sampler->Next2D();
                    Ray ray;
                    camera_->GenerateRay(pixelSample, ray);
                    Color3f value = integrator_->Li(scene_, sampler, ray);
                    res += value;
                }
                res /= sample_count_;
                block.Put(Vector2f(x + offset.x(), y + offset.y()), res);
            }
        }

    }
}
