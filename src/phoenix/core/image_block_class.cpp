#include "phoenix/core/image_block_class.h"
#include <spdlog/spdlog.h>

namespace Phoenix {
    ImageBlock::ImageBlock(Vector2i size) : size_(size), offset_(0, 0) {
        resize(size.y(), size.x());

    }

    void ImageBlock::Put(ImageBlock &b) {
        Vector2i offset = b.offset() - offset_;
        Vector2i size = b.GetSize();
        std::scoped_lock lock(mutex_);
        block(offset.y(), offset.x(), size.y(), size.x()) = b.topLeftCorner(size.y(), size.x());

    }

    void ImageBlock::Put(const Vector2f &_pos, const Color3f &value) {
        Vector2i pos(
                _pos.x() + 0.5f - (offset_.x()),
                _pos.y() + 0.5f - (offset_.y())
        );
        Vector2i min_t1(0, 0), max_t2((int) cols() - 1, (int) rows() - 1);
        pos = min_t1.cwiseMax(pos);
        pos = max_t2.cwiseMin(pos);
        coeffRef(pos.y(),pos.x())=value;
    }


    shared_ptr<Bitmap> ImageBlock::ToBitmap() {
        auto result = make_shared<Bitmap>(size_.x(), size_.y());
        for (int y = 0; y < size_.y(); ++y)
            for (int x = 0; x < size_.x(); ++x)
                result->SetColor(y, x, coeff(y,x));
        return result;
    }

    BlockGenerator::BlockGenerator(const Vector2i &size, int blockSize) : size_(size), block_size_(blockSize) {

        num_blocks_ = Vector2i(
                (int) std::ceil(size.x() / (float) blockSize),
                (int) std::ceil(size.y() / (float) blockSize));
        all_blocks_ = num_blocks_.x() * num_blocks_.y();
        blocks_left_ = num_blocks_.x() * num_blocks_.y();
        direction_ = ERight;
        block_ = Vector2i(num_blocks_ / 2);
        steps_left_ = 1;
        num_steps_ = 1;


    }

    bool BlockGenerator::Next(ImageBlock &block) {
        std::scoped_lock lock(mutex_);
        render_bar_.set_progress(((all_blocks_-blocks_left_)*100)/all_blocks_);

        Vector2i pos = block_ * block_size_;
        block.SetOffset(pos);
        block.SetSize((size_ - pos).cwiseMin(Vector2i::Constant(block_size_)));

        if (--blocks_left_ == 0)
            return true;

        do {
            switch (direction_) {
                case ERight:
                    ++block_.x();
                    break;
                case EDown:
                    ++block_.y();
                    break;
                case ELeft:
                    --block_.x();
                    break;
                case EUp:
                    --block_.y();
                    break;
            }

            if (--steps_left_ == 0) {
                direction_ = (direction_ + 1) % 4;
                if (direction_ == ELeft || direction_ == ERight)
                    ++num_steps_;
                steps_left_ = num_steps_;
            }
        } while ((block_.array() < 0).any() ||
                 (block_.array() >= num_blocks_.array()).any());

        return true;
    }

}
