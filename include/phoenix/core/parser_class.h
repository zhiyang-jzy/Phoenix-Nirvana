#pragma once
#include "phoenix/core/common.h"
#include "phoenix/utils/file_tool_class.h"
#include "render_class.h"
#include "ext/json.hpp"
namespace Phoenix{

    class Parser{
    private:
        PathTool path_tool_;
        string file_name_;
        map<string,shared_ptr<Bsdf>> bsdf_dic;

    public:
        void Parse(const string& filename, Renderer& render);
        std::filesystem::path GetParentPath() const{return path_tool_.current_path();};
        string file_name()const{return file_name_;}
    private:
        void ProcessCamera(const json& info, Renderer& render);
        void ProcessIntegrator(const json& info,Renderer& render);
        void ProcessSampler(const json& info,Renderer& render);
        void ProcessShapes(const json& info, Renderer& render);
        void ProcessEmitters(const json& info,Renderer& render);
        void ProcessBsdfs(const json& info);
        shared_ptr<Bsdf> ProcessBsdf(const json& info);
    };
}