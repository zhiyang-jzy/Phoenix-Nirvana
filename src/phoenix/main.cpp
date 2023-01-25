#include <iostream>
#include "ext/toml.hpp"
#include "phoenix/utils/file_tool_class.h"
#include "phoenix/core/bitmap_class.h"
#include "phoenix/utils/imgae_tool_class.h"
#include "phoenix/core/tracer_class.h"
#include "phoenix/core/object_class.h"
#include "phoenix/core/camera_class.h"

using namespace std;
using namespace Phoenix;

int main() {
    Phoenix::PathTool ft;
    int width = 400, height = 400;
    Bitmap s(width, height);
    ImageTool image_tool;
    Tracer tracer;

    tracer.AddSphere(Vector3f(0, 0, -5), 2);
    tracer.FinishAdd();
    PropertyList p;
    p.Set("width",width);
    p.Set("height",height);
    auto camera = std::dynamic_pointer_cast<Camera>(PhoenixObjectFactory::CreateInstance("pinhole", p));

    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            Ray ray;
            camera->GenerateRay({i,j},ray);
            auto res = tracer.TraceRay(ray);
            if(res.is_hit)
            {
                s.coeffRef(i,j) = Vector3f(1,1,1);
            }
            else{
                s.coeffRef(i,j) = Vector3f(0,0,0);
            }
        }
    }

    image_tool.write_exr(s, "test.exr");
}