#pragma once
#include <filesystem>
#include <string>
#include "phoenix/core/bitmap_class.h"

namespace Phoenix{
    class ImageTool{
        typedef std::filesystem::path Path;
    private:
        Path current_path_;
    public:

        ImageTool();
        explicit ImageTool(Path current_path);
        void write_exr(const Bitmap3f& bitmap, std::string filename);
        static std::shared_ptr<Bitmap> LoadImage(unsigned char* data,int width,int height,int channels);
        std::shared_ptr<Bitmap> PhoenixLoadImage(const std::string& filename);
    };
}
