#include "phoenix/core/bitmap_class.h"

#include <utility>

namespace Phoenix{

    void Bitmap::SetColor(uint x, uint y, Color4f color) {
        this->coeffRef(x,y) = std::move(color);
    }

    void Bitmap::SetColor(uint x, uint y, Color3f color) {
        this->coeffRef(x,y) = Color4f(color.x(),color.y(),color.z(),1.f);
    }

    Color4f Bitmap::GetColor(uint x, uint y) const {
        return this->coeff(x,y);
    }
}