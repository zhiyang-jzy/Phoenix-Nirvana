#pragma once

#include<filesystem>
#include <utility>
#include "phoenix/core/common.h"

namespace Phoenix
{
    class PathTool{
        typedef std::filesystem::path path;
    private:
        path current_path_;


    public:
        PathTool();
        [[nodiscard]] path current_path()const ;
        path GetFilePath(const string& filename){
            return (current_path_/path(filename)).lexically_normal() ;
        }
        void SetCurrentPath(path current_path){current_path_=std::move(current_path);}



    };
}