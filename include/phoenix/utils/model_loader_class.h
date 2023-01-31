#pragma once
#include "phoenix/utils/file_tool_class.h"
#include "phoenix/core/model_class.h"
namespace Phoenix{

    class ModelLoader{
    private:
        PathTool path_tool_;
    public:
        ModelLoader();
        ModelLoader(PathTool path_tool):path_tool_(path_tool){};
        shared_ptr<Model> Load(string filename);

    };
}