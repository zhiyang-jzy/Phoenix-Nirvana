#include "phoenix/utils/image_tool_class.h"
#include "phoenix/core/parser_class.h"
#include "phoenix/core/denoiser_class.h"
#include "ext/argparser.hpp"

using namespace std;
using namespace Phoenix;

int main(int argc, char const *argv[]) {

    argparse::ArgumentParser program("phoenix-nirvana","0.1");
    program.add_argument<std::string>("filepath")
            .help("json file path");
    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }
    
    auto filename = program.get<string>("filepath");

    Renderer render;
    Parser parser;
    parser.Parse(filename, render);

    render.SetMultiThread(true);
    render.Render();
    ImageTool img_tool(parser.GetParentPath());

    auto color = render.bitmap(ResType::Color)->ConvertTo3f();
    auto normal = render.bitmap(ResType::Normal)->ConvertTo3f();
    auto albedo = render.bitmap(ResType::Albedo)->ConvertTo3f();

    img_tool.write_exr(color, parser.file_name().append(".exr"));
    img_tool.write_exr(normal, parser.file_name().append("_normal.exr"));
    img_tool.write_exr(albedo, parser.file_name().append("_albedo.exr"));

    Denoiser denoiser;
    auto res = denoiser.Denoise(color, albedo, normal);
    img_tool.write_exr(res, parser.file_name().append("_denoised.exr"));


}
