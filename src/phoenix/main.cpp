#include <iostream>
#include "ext/toml.hpp"
#include "phoenix/utils/file_tool_class.h"
#include "phoenix/core/bitmap_class.h"
#include "phoenix/utils/imgae_tool_class.h"
using namespace std;
using namespace Phoenix;
int main()
{
    Phoenix::PathTool ft;
    int width=400,height = 500;
    Bitmap s(width,height);
    ImageTool image_tool;

    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            s.coeffRef(i,j) = Eigen::Vector3f(1,2,3);
        }
    }

    image_tool.write_exr(s,"test.exr");
}