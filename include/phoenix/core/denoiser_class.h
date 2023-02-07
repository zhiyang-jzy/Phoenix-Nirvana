#pragma once

#include<OpenImageDenoise/oidn.hpp>
#include "phoenix/core/bitmap_class.h"
namespace Phoenix{

    class Denoiser{
    private:
        oidn::DeviceRef device_;
        oidn::FilterRef filter_,albedo_filter_,normal_filter_;
    public:
        Denoiser();
        Bitmap3f Denoise(Bitmap3f color, Bitmap3f &albedo, Bitmap3f &normal);

    };

}