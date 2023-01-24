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
        void write_exr(const Bitmap& bitmap, std::string filename);
    };
}
