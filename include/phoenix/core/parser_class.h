#pragma once
#include "phoenix/core/common.h"
#include "render_class.h"
#include "ext/toml.hpp"
namespace Phoenix{

    class Parser{
    public:
        static void Parse(const string& filename, Renderer& render);
        static void ProcessCamera(const toml::parse_result& scene_config, Renderer& render);
        static void ProcessIntegrator(const toml::parse_result& scene_config, Renderer& render);
        static void ProcessObjects(const toml::parse_result& scene_config, Renderer& render);
        static void ProcessSampler(const toml::parse_result& scene_config, Renderer& render);
    };
}