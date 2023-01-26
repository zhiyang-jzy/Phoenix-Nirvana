#include <iostream>
#include "ext/toml.hpp"
#include "phoenix/core/bitmap_class.h"
#include "phoenix/utils/imgae_tool_class.h"
#include "phoenix/core/object_class.h"
#include "phoenix/core/camera_class.h"
#include "phoenix/core/scene_class.h"
#include "phoenix/core/integrator.h"
#include "phoenix/core/parser_class.h"
using namespace std;
using namespace Phoenix;

int main() {

    Renderer render;
    Parser::Parse("test.toml",render);
    ImageTool img_tool;
    render.Render();
    img_tool.write_exr(*render.bitmap(),"first.exr");
    int c;
}