#include "phoenix/utils/image_tool_class.h"
#define TINYEXR_USE_MINIZ 0
#include <zlib.h>
#define TINYEXR_IMPLEMENTATION
#include "ext/tinyexr.h"
#include <spdlog/spdlog.h>
//#define STB_IMAGE_IMPLEMENTATION
#include "ext/stb_image.h"
#undef LoadImage

namespace Phoenix{

    ImageTool::ImageTool(){
        current_path_ = std::filesystem::current_path();
    }
    ImageTool::ImageTool(Path current_path):current_path_(current_path) {

    }

    void ImageTool::write_exr(const Bitmap &bitmap, std::string filename) {
        EXRHeader header;
        InitEXRHeader(&header);

        EXRImage image;
        InitEXRImage(&image);

        int width = bitmap.width();
        int height = bitmap.height();
        auto rgb = reinterpret_cast<const float*>(bitmap.data());
        auto file_path = current_path_/filename;

        image.num_channels = 3;

        std::vector<float> images[3];
        images[0].resize(width * height);
        images[1].resize(width * height);
        images[2].resize(width * height);

        // Split RGBRGBRGB... into R, G and B layer
        for (int i = 0; i < width * height; i++) {
            images[0][i] = rgb[4 * i + 0];
            images[1][i] = rgb[4 * i + 1];
            images[2][i] = rgb[4 * i + 2];
        }

        float *image_ptr[3];
        image_ptr[0] = &(images[2].at(0)); // B
        image_ptr[1] = &(images[1].at(0)); // G
        image_ptr[2] = &(images[0].at(0)); // R

        image.images = (unsigned char **)image_ptr;
        image.width = width;
        image.height = height;

        header.num_channels = 3;
        header.channels =
                (EXRChannelInfo *)malloc(sizeof(EXRChannelInfo) * header.num_channels);
        // Must be (A)BGR order, since most of EXR viewers expect this channel order.
        strncpy(header.channels[0].name, "B", 255);
        header.channels[0].name[strlen("B")] = '\0';
        strncpy(header.channels[1].name, "G", 255);
        header.channels[1].name[strlen("G")] = '\0';
        strncpy(header.channels[2].name, "R", 255);
        header.channels[2].name[strlen("R")] = '\0';

        header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
        header.requested_pixel_types =
                (int *)malloc(sizeof(int) * header.num_channels);
        for (int i = 0; i < header.num_channels; i++) {
            header.pixel_types[i] =
                    TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
            header.requested_pixel_types[i] =
                    TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in
            // .EXR
        }

        const char *err = nullptr;
        int ret = SaveEXRImageToFile(&image, &header, file_path.string().c_str(), &err);
        if (ret != TINYEXR_SUCCESS) {
            spdlog::error("save EXR err: {}",err);

            FreeEXRErrorMessage(err); // free's buffer for an error message
            return;
        }
        spdlog::info("Saved exr file to {} \n", file_path.string());

        free(header.channels);
        free(header.pixel_types);
        free(header.requested_pixel_types);
    }

    std::shared_ptr<Bitmap> ImageTool::LoadImage(const std::string& filename) {
        auto file_path = current_path_/filename;
        int channels,width,height;
        auto data = stbi_load(file_path.string().c_str(),&width,&height,&channels,STBI_default);
        return LoadImage(data,width,height,channels);

    }

    std::shared_ptr<Bitmap> ImageTool::LoadImage(unsigned char *data, int width, int height, int channels) {
        auto bitmap = std::make_shared<Bitmap>(width,height);
        int byte_per_scanline = channels*width;
        constexpr float color_scale = 1.f/255.f;
        Color4f now_color(1,1,1,1);
        for(uint i=0;i<width;i++){
            for(uint j=0;j<height;j++){
                auto pixel = data+j*byte_per_scanline+i*channels;
                for(uint k=0;k<channels;k++){
                    now_color[k]=static_cast<float>(pixel[k])*color_scale;
                }
                bitmap->SetColor(j,i,now_color);
            }
        }
        return bitmap;
    }


}