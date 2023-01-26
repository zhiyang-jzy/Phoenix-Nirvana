#include "phoenix/core/parser_class.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <iostream>
#include <magic_enum.hpp>

#include "ext/toml.hpp"
#include "phoenix/utils/file_tool_class.h"
#include "phoenix/core/object_class.h"

namespace Phoenix {


    Vector3f ConvertToVector3f(const vector<double> &v) {
        return {v[0], v[1], v[2]};
    }

    PropertyList GenProperList(const toml::impl::wrap_node<toml::table> *table) {
        PropertyList properties;
        table->for_each([&](auto &key, auto &v) {
            auto now_key = static_cast<string>(key);
            if constexpr (toml::is_value<decltype(v)>)
                properties.Set(now_key, v.get());
            else if constexpr (toml::is_array<decltype(v)>) {
                vector<double> temp_v;
                v.for_each([&](auto &v) {
                    if constexpr (toml::is_number<decltype(v)>)
                        temp_v.push_back(static_cast<double>(v.get()));
                });
                if (v.size() == 3) {
                    auto value = ConvertToVector3f(temp_v);
                    properties.Set(now_key, value);
                } else if (v.size() == 9) {

                }
            }
        });
        return properties;
    }

    void CheckKeyExist(const toml::parse_result &result, const string &key) {
        if (!result.contains(key)) {
            spdlog::error("scene file has no key {}", key);
            exit(0);
        }
    }


    void Parser::Parse(const string &filename, Renderer &render) {
        PathTool path_tool;
        auto file_path = path_tool.get_file_path(filename);
        if (!exists(file_path)) {
            spdlog::error("no such file: {} ", absolute(file_path).string());
            exit(0);
        }
        spdlog::info("reading scene from {}", absolute(file_path).string());

        auto scene_config = toml::parse_file(file_path.string());

        ProcessCamera(scene_config, render);
        ProcessIntegrator(scene_config, render);
        ProcessObjects(scene_config, render);
        ProcessSampler(scene_config, render);

    }

    void Parser::ProcessCamera(const toml::parse_result &scene_config, Renderer &render) {
        CheckKeyExist(scene_config, "camera");
        auto camera_info = scene_config["camera"].as_table();
        string name = (*camera_info)["type"].value_or("");
        CheckFactoryExist(name);
        PropertyList camera_properties = GenProperList(camera_info);
        auto camera = std::dynamic_pointer_cast<Camera>(PhoenixObjectFactory::CreateInstance(name, camera_properties));
        render.SetCamera(camera);
        render.SetPicSize(camera->output_size_);


    }

    void Parser::ProcessIntegrator(const toml::parse_result &scene_config, Renderer &render) {
        CheckKeyExist(scene_config, "integrator");
        auto camera_info = scene_config["integrator"].as_table();
        string name = (*camera_info)["type"].value_or("");
        CheckFactoryExist(name);
        PropertyList integrator_properties = GenProperList(camera_info);
        auto integrator = std::dynamic_pointer_cast<Integrator>(
                PhoenixObjectFactory::CreateInstance(name, integrator_properties));
        render.SetIntegrator(integrator);

    }

    void Parser::ProcessObjects(const toml::parse_result &scene_config, Renderer &render) {
        auto scene = make_shared<Scene>();
        CheckKeyExist(scene_config, "shapes");
        auto objects = scene_config["shapes"].as_array();
        objects->for_each([&](auto &v) {
            auto value = v.as_table();
            PropertyList shape_properties = GenProperList(value);
            string name = (*value)["type"].value_or("");
            CheckFactoryExist(name);
            auto shape = std::dynamic_pointer_cast<Shape>(PhoenixObjectFactory::CreateInstance(name, shape_properties));
            scene->AddShape(shape);
        });
        scene->FinishAdd();
        render.SetScene(scene);
    }

    void Parser::ProcessSampler(const toml::parse_result &scene_config, Renderer &render) {
        auto sampler = std::dynamic_pointer_cast<Sampler>(PhoenixObjectFactory::CreateInstance("independent", {}));
        render.SetSampler(sampler);
    }
}