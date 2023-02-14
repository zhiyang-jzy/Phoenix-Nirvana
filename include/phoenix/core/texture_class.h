#include <utility>

#include "common_struct.h"
#include "bitmap_class.h"

namespace Phoenix {
    enum class TextureType{
        Diffuse
    };
    class Texture {
    public:
        [[nodiscard]] virtual Color3f GetColor(const Vector2f &uv) const = 0;
    };

    class ImageTexture : public Texture {
    private:
        shared_ptr<Bitmap> bitmap_;
    public:
        explicit ImageTexture(shared_ptr<Bitmap> bitmap) : bitmap_(std::move(bitmap)) {};

        [[nodiscard]] Color3f GetColor(const Vector2f &uv) const override;
    };

    class SingleColorTexture : public Texture{
    private:
        Color3f color_;
    public:
        explicit SingleColorTexture(Color3f color): color_(color){}
        Color3f GetColor(const Vector2f &uv) const override;
    };
}