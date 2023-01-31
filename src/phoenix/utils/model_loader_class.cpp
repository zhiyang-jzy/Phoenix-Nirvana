#include "phoenix/utils/model_loader_class.h"
#include <spdlog/spdlog.h>
namespace Phoenix{

    ModelLoader::ModelLoader() {

    }

    shared_ptr<Model> ModelLoader::Load(string filename) {
        auto file_path = path_tool_.GetFilePath(filename);
        if(!exists(file_path)){
            spdlog::error("no such model file: {}",file_path.string());
            exit(0);
        }
        path_tool_.SetCurrentPath(file_path.parent_path());

        auto model = make_shared<Model>();

    }
}