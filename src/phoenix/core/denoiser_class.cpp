#include "phoenix/core/denoiser_class.h"
#include <spdlog/spdlog.h>

namespace Phoenix {

    Denoiser::Denoiser() {
        device_ = oidn::newDevice();
        device_.commit();

        filter_ = device_.newFilter("RT");


    }

    Bitmap3f Denoiser::Denoise(Bitmap3f color, Bitmap3f &albedo, Bitmap3f &normal) {

        Bitmap3f res(color.width(), color.height());

        filter_.setImage("color", reinterpret_cast<void *>(color.data()), oidn::Format::Float3, color.width(),
                         color.height());
        filter_.setImage("albedo", reinterpret_cast<void *>(albedo.data()), oidn::Format::Float3, color.width(),
                         color.height());
        filter_.setImage("normal", reinterpret_cast<void *>(normal.data()), oidn::Format::Float3, color.width(),
                         color.height());
        filter_.setImage("output", reinterpret_cast<void *>(res.data()), oidn::Format::Float3, color.width(),
                         color.height());
        filter_.set("hdr", true); // beauty image is HDR
        //filter_.set("cleanAux", true); // auxiliary images will be prefiltered

        filter_.commit();
        filter_.execute();

        const char *errorMessage;
        if (device_.getError(errorMessage) != oidn::Error::None)
            spdlog::error("oidn error: {}", errorMessage);
        return res;
    }
}