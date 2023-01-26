#include <iostream>
#include "ext/toml.hpp"
#include "phoenix/core/bitmap_class.h"
#include "phoenix/utils/imgae_tool_class.h"
#include "phoenix/core/object_class.h"
#include "phoenix/core/camera_class.h"
#include "phoenix/core/scene_class.h"
#include "phoenix/core/integrator.h"
using namespace std;
using namespace Phoenix;

int main() {
    int width = 256, height = 256;
    Bitmap s(width, height);
    ImageTool image_tool;

    PropertyList p;
    p.Set("width", width);
    p.Set("height", height);
    p.Set("fov",34.2f);
    p.Set("center",Vector3f(0,0,-10));

    auto sphere = std::dynamic_pointer_cast<Shape>(PhoenixObjectFactory::CreateInstance("sphere", p));
    auto camera = std::dynamic_pointer_cast<Camera>(PhoenixObjectFactory::CreateInstance("pinhole", p));
    auto sampler = std::dynamic_pointer_cast<Sampler>(PhoenixObjectFactory::CreateInstance("independent",p));
    auto scene = make_shared<Scene>();
    auto integrator = std::dynamic_pointer_cast<Integrator>(PhoenixObjectFactory::CreateInstance("test", p));
    scene->AddShape(sphere);
    scene->FinishAdd();
    Ray ray;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            camera->GenerateRay({j,i},ray);
            auto res = integrator->Li(scene,sampler,ray);
            s.coeffRef(i,j) = res;
        }
    }
    image_tool.write_exr(s, "test.exr");
}