#include <iostream>
#include "ext/toml.hpp"
#include "phoenix/utils/file_tool_class.h"
#include "phoenix/core/bitmap_class.h"
#include "phoenix/utils/imgae_tool_class.h"
#include "phoenix/core/tracer_class.h"
using namespace std;
using namespace Phoenix;
int main()
{
    Phoenix::PathTool ft;
    int width=400,height = 400;
    Bitmap s(width,height);
    ImageTool image_tool;
    Tracer tracer;

    tracer.AddSphere(Vector3f(0,0,-5),2);
    tracer.FinishAdd();

    Vector3f orig(0.f,0.f,0.f);


    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            float x = (float(i)/height)*2.f-1.f;
            float y = (float(j)/width)*2.f-1.f;
            Vector3f dir = Vector3f(x,y,-1).normalized();
            Ray ray{orig,dir,kEpsilon,kFInf};

            auto res = tracer.TraceRay(ray);

            if(res.hit.geomID!= RTC_INVALID_GEOMETRY_ID)
                s.coeffRef(i,j) = Eigen::Vector3f(1,1,0);
            else
                s.coeffRef(i,j) = Eigen::Vector3f(0.2,0.2,0.2);
        }
    }

    image_tool.write_exr(s,"test.exr");
}