#pragma once

#include<filesystem>
#include "phoenix/core/common.h"

namespace Phoenix
{
    class PathTool{
        typedef std::filesystem::path path;
    private:
        path current_path_;


    public:
        PathTool();
        path current_path();
        path get_file_path(const string& filename){
            return (current_path_/path(filename)).lexically_normal() ;
        }



    };
}