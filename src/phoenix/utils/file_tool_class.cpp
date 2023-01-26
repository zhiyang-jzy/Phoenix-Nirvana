#include "phoenix/utils/file_tool_class.h"

namespace Phoenix{


    PathTool::PathTool() {
        current_path_ = std::filesystem::current_path();
    }

    PathTool::path PathTool::current_path() {
        return current_path_;
    }
}