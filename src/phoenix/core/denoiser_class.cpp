#include "phoenix/core/denoiser_class.h"
#include <spdlog/spdlog.h>

namespace Phoenix {

    Denoiser::Denoiser() {
        device_ = oidn::newDevice();
        device_.commit();

        filter_ = device_.newFilter("RT");
        albedo_filter_ = device_.newFilter("RT");
        normal_filter_ = device_.newFilter("RT");


    }

    Bitmap3f Denoiser::Denoise(Bitmap3f color, Bitmap3f &albedo, Bitmap3f &normal) {

        Bitmap3f res(color.width(), color.height());

        auto color_ptr = reinterpret_cast<void *>(color.data());
        auto albedo_ptr = reinterpret_cast<void *>(albedo.data());
        auto normal_ptr = reinterpret_cast<void *>(normal.data());
        auto res_ptr = reinterpret_cast<void *>(res.data());

        filter_.setImage("color", color_ptr, oidn::Format::Float3, color.width(),
                         color.height());
        filter_.setImage("albedo", albedo_ptr, oidn::Format::Float3, color.width(),
                         color.height());
        filter_.setImage("normal", normal_ptr, oidn::Format::Float3, color.width(),
                         color.height());
        filter_.setImage("output", res_ptr, oidn::Format::Float3, color.width(),
                         color.height());
        filter_.set("hdr", true); // beauty image is HDR
        filter_.set("cleanAux", true); // auxiliary images will be prefiltered
        filter_.commit();

        albedo_filter_.setImage("albedo", albedo_ptr, oidn::Format::Float3, color.width(),
                         color.height());
        albedo_filter_.setImage("output", albedo_ptr, oidn::Format::Float3, color.width(),
                         color.height());
        albedo_filter_.commit();


        normal_filter_.setImage("albedo", normal_ptr, oidn::Format::Float3, color.width(),
                                color.height());
        normal_filter_.setImage("output", normal_ptr, oidn::Format::Float3, color.width(),
                                color.height());
        normal_filter_.commit();


        albedo_filter_.execute();
        albedo_filter_.execute();
        filter_.execute();

        const char *errorMessage;
        if (device_.getError(errorMessage) != oidn::Error::None)
            spdlog::error("oidn error: {}", errorMessage);
        return res;
    }
}