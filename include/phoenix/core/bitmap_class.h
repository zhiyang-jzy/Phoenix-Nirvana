#pragma once
#include <Eigen/Eigen>
namespace Phoenix{
    class Bitmap:public Eigen::Array<Eigen::Vector3f,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> {
        typedef Eigen::Array<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Base;
    private:
        int width_,height_;
    public:
        Bitmap(int width,int height):Base(height,width),width_(width),height_(height){}\
        [[nodiscard]] int width() const{return width_;}
        [[nodiscard]] int height() const{return height_;}

    };
}