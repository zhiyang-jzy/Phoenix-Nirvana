#pragma once
#include "phoenix/core/common.h"
#include "phoenix/utils/file_tool_class.h"
#include "render_class.h"
#include "ext/toml.hpp"
namespace Phoenix{

    class Parser{
    private:
        PathTool path_tool_;
        string file_name_;

    public:
        void Parse(const string& filename, Renderer& render);
        std::filesystem::path GetParentPath() const{return path_tool_.current_path();};
        string file_name()const{return file_name_;}
    private:
        void ProcessCamera(const toml::parse_result& scene_config, Renderer& render);
        void ProcessIntegrator(const toml::parse_result& scene_config, Renderer& render);
        void ProcessObjects(const toml::parse_result& scene_config, Renderer& render);
        void ProcessEmitters(const toml::parse_result& scene_config, Renderer& render);
        void ProcessSampler(const toml::parse_result& scene_config, Renderer& render);
    };
}