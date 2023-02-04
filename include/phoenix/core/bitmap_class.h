#pragma once
#include <Eigen/Eigen>
#include "common.h"
#include "vector_class.h"

namespace Phoenix{
    class Bitmap:public Eigen::Array<Color4f,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> {
        typedef Eigen::Array<Color4f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Base;
    private:
        int width_,height_;
    public:
        Bitmap(int width,int height):Base(height,width),width_(width),height_(height){}\
        [[nodiscard]] int width() const{return width_;}
        [[nodiscard]] int height() const{return height_;}
        void SetColor(uint x,uint y,Color4f color);
        void SetColor(uint x,uint y,Color3f color);
        Color4f GetColor(uint x,uint y) const;


    };
}