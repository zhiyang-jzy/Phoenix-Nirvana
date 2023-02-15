#pragma once

#include <utility>

#include "common_struct.h"
#include "bitmap_class.h"
#include "object_class.h"

namespace Phoenix {
    enum class TextureType{
        Diffuse
    };

    class Texture : public PhoenixObject {
    public:
        string name_;

        [[nodiscard]] virtual Color3f GetColor(const Vector2f &uv) const = 0;

        virtual Color3f GetAverage() const = 0;

        string name() const { return name_; }

        PClassType GetClassType() const override { return PClassType::PTexture; }

        string ToString() const override { return "texture"; }

    };

    class ImageTexture : public Texture {
    private:
        shared_ptr<Bitmap> bitmap_;
    public:
        ImageTexture(shared_ptr<Bitmap> bitmap, string name) : bitmap_(
                std::move(bitmap)) { name_ = std::move(name); };

        Color3f GetAverage() const override { return bitmap_->GetAverage(); }

        [[nodiscard]] Color3f GetColor(const Vector2f &uv) const override;
    };

    class SingleColorTexture : public Texture {
    private:
        Color3f color_;
    public:
        SingleColorTexture(Color3f color, string name) : color_(std::move(color)) { name_ = name; }

        Color3f GetAverage() const override { return color_; }

        Color3f GetColor(const Vector2f &uv) const override;
    };
}