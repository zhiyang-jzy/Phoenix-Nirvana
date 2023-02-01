#include "phoenix/core/parser_class.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <iostream>
#include <magic_enum.hpp>
#include <utility>

#include "ext/toml.hpp"
#include "phoenix/utils/file_tool_class.h"
#include "phoenix/core/object_class.h"
#include "phoenix/core/emitter_class.h"

namespace Phoenix {


    Vector3f ConvertToVector3f(const vector<double> &v) {
        return Vector3f(v[0], v[1], v[2]);
    }
    string GenFileName(string name){
        string outputName = std::move(name);
        size_t lastdot = outputName.find_last_of('.');
        if (lastdot != std::string::npos)
            outputName.erase(lastdot, std::string::npos);
        return outputName;
    }

    PropertyList GenProperList(const toml::impl::wrap_node<toml::table> *table) {
        PropertyList properties;
        table->for_each([&](auto &key, auto &v) {
            auto now_key = static_cast<string>(key);
            if constexpr (toml::is_value<decltype(v)>)
            {
                properties.Set(now_key, v.get());
            }
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
    shared_ptr<Shape> ProcessShape(const toml::impl::wrap_node<toml::table> *table)
    {
        PropertyList shape_properties = GenProperList(table);
        string name = (*table)["type"].value_or("");
        CheckFactoryExist(name);
        auto shape = std::dynamic_pointer_cast<Shape>(PhoenixObjectFactory::CreateInstance(name, shape_properties));
        return shape;
    }

    shared_ptr<Bsdf> ProcessBsdf(const toml::impl::wrap_node<toml::table> *table)
    {
        PropertyList bsdf_properties = GenProperList(table);
        string name = (*table)["type"].value_or("");
        CheckFactoryExist(name);
        auto bsdf = std::dynamic_pointer_cast<Bsdf>(PhoenixObjectFactory::CreateInstance(name, bsdf_properties));
        return bsdf;
    }

    void CheckKeyExist(const toml::parse_result &result, const string &key) {
        if (!result.contains(key)) {
            spdlog::error("scene file has no key {}", key);
            exit(0);
        }
    }


    void Parser::Parse(const string &filename, Renderer &render) {
        auto file_path = path_tool_.GetFilePath(filename);
        if (!exists(file_path)) {
            spdlog::error("no such file: {} ", absolute(file_path).string());
            exit(0);
        }
        file_name_ = GenFileName(file_path.filename().string());
        path_tool_.SetCurrentPath(file_path.parent_path());
        std::filesystem::current_path(file_path.parent_path());
        spdlog::info("now path: {}",std::filesystem::current_path().string());
        spdlog::info("reading scene from {}", absolute(file_path).string());
        auto scene_config = toml::parse_file(file_path.string());

        render.SetSampleCount(scene_config["sample_count"].value_or(16));

        ProcessCamera(scene_config, render);
        ProcessIntegrator(scene_config, render);
        ProcessEmitters(scene_config,render);
        ProcessObjects(scene_config, render);
        ProcessSampler(scene_config, render);
        render.scene()->PostProcess();

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
        auto scene = render.scene();
        CheckKeyExist(scene_config, "shapes");
        auto objects = scene_config["shapes"].as_array();
        objects->for_each([&](auto &v) {
            auto value = v.as_table();
            auto shape = ProcessShape(value);
            if(!value->contains("bsdf"))
            {
                spdlog::error("shape has no bsdf!");
                exit(0);
            }
            auto bsdf_table = (*value)["bsdf"].as_table();
            auto bsdf = ProcessBsdf(bsdf_table);
            shape->AddChild(bsdf);
            scene->AddShape(shape);
        });
        scene->FinishAdd();
    }

    void Parser::ProcessSampler(const toml::parse_result &scene_config, Renderer &render) {
//        auto sampler = std::dynamic_pointer_cast<Sampler>(PhoenixObjectFactory::CreateInstance("independent", {}));
//        render.SetSampler(sampler);
        CheckKeyExist(scene_config,"sampler");
        auto sampler_info = scene_config["sampler"].as_table();
        string name = (*sampler_info)["type"].value_or("");
        CheckFactoryExist(name);
        PropertyList integrator_properties = GenProperList(sampler_info);
        auto sampler = std::dynamic_pointer_cast<Sampler>(
                PhoenixObjectFactory::CreateInstance(name, integrator_properties));
        render.SetSampler(sampler);

    }

    void Parser::ProcessEmitters(const toml::parse_result &scene_config, Renderer &render) {
        auto scene = render.scene();
        CheckKeyExist(scene_config, "emitters");
        auto emitters = scene_config["emitters"].as_array();
        emitters->for_each([&](auto &v) {
            auto value = v.as_table();
            PropertyList emitter_properties = GenProperList(value);
            string name = (*value)["type"].value_or("");
            CheckFactoryExist(name);
            auto emitter = std::dynamic_pointer_cast<Emitter>(PhoenixObjectFactory::CreateInstance(name, emitter_properties));
            if(value->contains("shape"))
            {
                auto shape_info = (*value)["shape"].as_table();
                auto shape = ProcessShape(shape_info);

                emitter->AddChild(shape);
            }
            scene->AddEmitter(emitter);
        });
        scene->FinishAdd();
    }
}