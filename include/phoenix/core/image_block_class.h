#pragma once

#include <mutex>

#include "phoenix/core/common.h"
#include "ext/indicators.hpp"
#include "bitmap_class.h"
#include "vector_class.h"

namespace Phoenix {
    class ImageBlock : public Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
    protected:
        Vector2i offset_, size_;
        mutable std::mutex mutex_;
    public:
        explicit ImageBlock(Vector2i size);

        void SetOffset(Vector2i offset) { offset_ = std::move(offset); }

        Vector2i offset() const { return offset_; }

        void Put(ImageBlock &b);

        void Put(const Vector2f &pos, const Color3f &value);

        Vector2i GetSize() const { return size_; }

        void SetSize(Vector2i size){size_=std::move(size);};

        void Clear() { setConstant(Color3f(0, 0, 0)); }

        shared_ptr<Bitmap> ToBitmap();

    };

    class BlockGenerator {

    private:
        indicators::ProgressBar render_bar_{
                indicators::option::BarWidth{50},
                indicators::option::Start{" ["},
                indicators::option::Fill{"="},
                indicators::option::Lead{"="},
                indicators::option::Remainder{"-"},
                indicators::option::End{"]"},
                indicators::option::PrefixText{"Rendering"},
                indicators::option::ForegroundColor{indicators::Color::yellow},
                indicators::option::ShowElapsedTime{true},
                indicators::option::ShowRemainingTime{true},
                indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
        };
    public:

        explicit BlockGenerator(const Vector2i &size, int blockSize=kBlockSize);

        bool Next(ImageBlock &block);
        [[nodiscard]] int GetBlockCount() const { return blocks_left_; }

        static constexpr int kBlockSize = 32;
    protected:
        enum EDirection { ERight = 0, EDown, ELeft, EUp };

        Vector2i block_;
        Vector2i num_blocks_;
        Vector2i size_;
        int block_size_;
        int all_blocks_;
        int num_steps_;
        int blocks_left_;
        int steps_left_;
        int direction_;
        std::mutex mutex_;
    };

}