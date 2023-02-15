#include "phoenix/core/texture_class.h"

namespace Phoenix {

    float frac(float x) {
        if (x >= 0)
            return x - int(x);
        else
            return x - int(x) + 1;
    }

    Color3f ImageTexture::GetColor(const Vector2f &uv) const {
        float u = uv.x(), v = uv.y();
        u = frac(u), v = frac(v);
        int width = bitmap_->width(), height = bitmap_->height();


        u = Clamp(u, 0, 1);
        v = Clamp(v, 0, 1);
        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);
        if (i >= width) i = width - 1;
        if (j >= height) j = height - 1;

        return bitmap_->GetColor3f(j, i);
    }

    Color3f SingleColorTexture::GetColor(const Vector2f &uv) const {
        return color_;
    }
}