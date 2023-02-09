#include "phoenix/core/parser_class.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <iostream>

#include <fstream>
#include "ext/json.hpp"

using json = nlohmann::json;

#include "phoenix/core/object_class.h"
#include "phoenix/core/emitter_class.h"

namespace Phoenix {

    template<typename T>
    shared_ptr<T> CreateInstance(string name, const PropertyList &proper) {
        return std::dynamic_pointer_cast<T>(PhoenixObjectFactory::CreateInstance(name, proper));
    }

    PropertyList GenPropertyList(const json &info) {
        PropertyList list;
        for (auto &[key, value]: info.items()) {
            list.Set(key, value);
        }
        return list;
    }


    Vector3f ConvertToVector3f(const vector<double> &v) {
        return Vector3f(v[0], v[1], v[2]);
    }

    string GenFileName(string name) {
        string outputName = std::move(name);
        size_t lastdot = outputName.find_last_of('.');
        if (lastdot != std::string::npos)
            outputName.erase(lastdot, std::string::npos);
        return outputName;
    }

    shared_ptr<Bsdf> Parser::ProcessBsdf(const json &info) {
        auto property = GenPropertyList(info);
        auto name = info["type"].get<string>();
        CheckFactoryExist(name);
        return CreateInstance<Bsdf>(name, property);
    }

    shared_ptr<Shape> ProcessShape(const json& info){
        auto property = GenPropertyList(info);
        auto name = info["type"].get<string>();
        CheckFactoryExist(name);
        return CreateInstance<Shape>(name, property);
    }
    shared_ptr<Emitter> ProcessEmitter(const json& info){
        auto property = GenPropertyList(info);
        auto name = info["type"].get<string>();
        CheckFactoryExist(name);
        return CreateInstance<Emitter>(name, property);
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
        spdlog::info("now path: {}", std::filesystem::current_path().string());
        spdlog::info("reading scene from {}", absolute(file_path).string());

        std::ifstream file(file_path);
        json scene_config = json::parse(file);


        render.SetSampleCount(scene_config["sample_count"].get<int>());

        ProcessCamera(scene_config, render);
        ProcessIntegrator(scene_config, render);
        ProcessSampler(scene_config, render);
        ProcessEmitters(scene_config,render);
        ProcessShapes(scene_config,render);

        render.scene()->PostProcess();

    }

    void Parser::ProcessCamera(const json &info, Renderer &render) {
        if (!info.contains("camera")) {
            spdlog::error("no camera!");
            exit(0);
        }
        auto camera_info = info["camera"];
        auto camera_type = camera_info["type"].get<string>();
        CheckFactoryExist(camera_type);
        auto camera = CreateInstance<Camera>(camera_type, GenPropertyList(camera_info));
        render.SetCamera(camera);
        render.SetPicSize(camera->output_size_);

    }

    void Parser::ProcessIntegrator(const json &info, Renderer &render) {
        if (!info.contains("integrator")) {
            spdlog::error("no integrator!");
            exit(0);
        }
        auto camera_info = info["integrator"];
        auto camera_type = camera_info["type"].get<string>();
        CheckFactoryExist(camera_type);
        auto integrator = CreateInstance<Integrator>(camera_type, GenPropertyList(camera_info));
        render.SetIntegrator(integrator);
    }

    void Parser::ProcessSampler(const json &info, Renderer &render) {
        if (!info.contains("sampler")) {
            spdlog::error("no sampler!");
            exit(0);
        }
        auto camera_info = info["sampler"];
        auto camera_type = camera_info["type"].get<string>();
        CheckFactoryExist(camera_type);
        auto sampler = CreateInstance<Sampler>(camera_type, GenPropertyList(camera_info));
        render.SetSampler(sampler);
    }

    void Parser::ProcessShapes(const json &info, Renderer &render) {
        auto scene = render.scene();
        if (!info.contains("shapes"))
            return;
        auto objects = info["shapes"];
        for (auto& el : objects.items()) {
            auto shape_info = el.value();
            auto shape = ProcessShape(shape_info);
            if(!shape_info.contains("bsdf"))
            {
                spdlog::error("no bsdf!");
                exit(0);
            }
            auto bsdf = ProcessBsdf(shape_info["bsdf"]);
            shape->AddChild(bsdf);
            scene->AddShape(shape);
        }
    }

    void Parser::ProcessEmitters(const json &info, Renderer &render) {
        auto scene = render.scene();
        if(!info.contains("emitters"))
        {
            spdlog::error("no emitter");
            return;
        }
        auto emitters = info["emitters"];
        for (auto& el : emitters.items()) {
            auto emitter_info = el.value();
            auto emitter = ProcessEmitter(emitter_info);
            if(emitter_info.contains("shape")){
                auto shape = ProcessShape(emitter_info["shape"]);
                emitter->AddChild(shape);
            }
            scene->AddEmitter(emitter);
        }
    }


}