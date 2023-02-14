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

    Bitmap3f Bitmap::ConvertTo3f() {
        Bitmap3f res(width_,height_);
        for(int i=0;i<height_;i++){
            for(int j=0;j<width_;j++){
                auto pre = coeff(i,j);
                res.coeffRef(i,j) = Color3f(pre.x(),pre.y(),pre.z());
            }
        }
        return res;
    }

    Color3f Bitmap::GetColor3f(uint x, uint y) const {
        auto c = this->coeff(x,y);
        return {c.x(), c.y(), c.z()};
    }
}