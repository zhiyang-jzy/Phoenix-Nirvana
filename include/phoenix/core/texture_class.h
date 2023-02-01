#include <utility>

#include "common_struct.h"
#include "bitmap_class.h"

namespace Phoenix {
    enum class TextureType{
        Diffuse
    };
    class Texture {
    public:
        [[nodiscard]] virtual Color4f GetColor(const Vector2f &uv) const { return {1, 1, 0, 1}; }
    };

    class ImageTexture : public Texture {
    private:
        shared_ptr<Bitmap> bitmap_;
    public:
        explicit ImageTexture(shared_ptr<Bitmap> bitmap) : bitmap_(std::move(bitmap)) {};

        [[nodiscard]] Color4f GetColor(const Vector2f &uv) const override;
    };
}