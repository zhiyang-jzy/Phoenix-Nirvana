#include "phoenix/utils/image_tool_class.h"
#include "phoenix/core/parser_class.h"
#include "phoenix/utils/file_tool_class.h"
using namespace std;
using namespace Phoenix;

int main() {

    Renderer render;
    Parser parser;
    parser.Parse("scene/cbox/cbox.toml",render);

    render.SetMultiThread(false);
    render.Render();
    ImageTool img_tool(parser.GetParentPath());
    img_tool.write_exr(*render.bitmap(),parser.file_name().append(".exr"));

}
