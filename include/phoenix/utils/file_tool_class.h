#pragma once

#include<filesystem>

namespace Phoenix
{
    class PathTool{
        typedef std::filesystem::path Path;
    private:
        Path current_path_;


    public:
        PathTool();
        Path current_path();




    };
}