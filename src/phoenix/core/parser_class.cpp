#include "phoenix/core/parser_class.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <iostream>

#include <fstream>
#include "ext/json.hpp"

using json = nlohmann::json;

#include "phoenix/core/object_class.h"
#include "phoenix/core/emitter_class.h"
#include "phoenix/utils/tool_function.h"
#include "phoenix/utils/image_tool_class.h"

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
        if (info.contains("ref")) {
            auto name = info.at("ref").get<string>();
            return bsdf_dic[name];
        }
        auto property = GenPropertyList(info);
        auto name = info["type"].get<string>();
        CheckFactoryExist(name);
        return CreateInstance<Bsdf>(name, property);
    }

    Transform ProcessMatrix(const json &info) {
        auto vec = info["value"].get<vector<float>>();
        Eigen::Matrix4f mat;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                mat.coeffRef(i, j) = vec[i * 4 + j];
        return Transform(mat);

    }

    Transform ProcessToWorld(const json &info) {
        Eigen::Affine3f af = Eigen::Affine3f::Identity();
        if (info.contains("scale")) {
            auto vec = info["scale"].get<vector<float>>();
            Vector3f rot;
            rot = vec;
            af.prescale(rot);
        }

        if (info.contains("rotation")) {
            auto vec = info["rotation"].get<vector<float>>();
            std::for_each(vec.begin(), vec.end(), [&](auto &item) {
                item = DegToRad(item);
            });
            af.prerotate(Eigen::AngleAxisf(vec[2], Vector3f::UnitZ()));
            af.prerotate(Eigen::AngleAxisf(vec[1], Vector3f::UnitY()));
            af.prerotate(Eigen::AngleAxisf(vec[0], Vector3f::UnitX()));

        }
        if (info.contains("translate")) {
            auto vec = info["translate"].get<vector<float>>();
            Vector3f rot;
            rot = vec;
            af.pretranslate(rot);
        }
        return Transform(af.matrix());
    }


    Transform ProcessTransform(const json &info) {
        if (info["type"].get<string>() == "matrix") {
            return ProcessMatrix(info);
        }
        return ProcessToWorld(info);
    }

    shared_ptr<Shape> ProcessShape(const json &info) {
        auto property = GenPropertyList(info);
        auto name = info["type"].get<string>();
        CheckFactoryExist(name);
        auto shape = CreateInstance<Shape>(name, property);
        if (info.contains("transform")) {
            auto transform = ProcessTransform(info["transform"]);
            shape->ApplyTransform(transform);
        }
        return shape;

    }

    shared_ptr<Emitter> ProcessEmitter(const json &info) {
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
        img_tool_ = make_shared<ImageTool>(path_tool_.current_path());
        std::filesystem::current_path(file_path.parent_path());
        spdlog::info("now path: {}", std::filesystem::current_path().string());
        spdlog::info("reading scene from {}", absolute(file_path).string());

        std::ifstream file(file_path);
        json scene_config = json::parse(file);


        render.SetSampleCount(scene_config["sample_count"].get<int>());

        ProcessCamera(scene_config, render);
        ProcessBsdfs(scene_config);
        ProcessIntegrator(scene_config, render);
        ProcessSampler(scene_config, render);
        ProcessEmitters(scene_config, render);
        ProcessShapes(scene_config, render);

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
        for (auto &el: objects.items()) {
            auto shape_info = el.value();
            auto shape = ProcessShape(shape_info);
            if (!shape_info.contains("bsdf")) {
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
        if (!info.contains("emitters")) {
            spdlog::error("no emitter");
            return;
        }
        auto emitters = info["emitters"];
        for (auto &el: emitters.items()) {
            auto emitter_info = el.value();
            auto emitter = ProcessEmitter(emitter_info);
            if (emitter_info.contains("shape")) {
                auto shape = ProcessShape(emitter_info["shape"]);
                emitter->AddChild(shape);
            }
            scene->AddEmitter(emitter);
        }
    }

    void Parser::ProcessBsdfs(const json &info) {
        if (!info.contains("bsdfs")) {
            spdlog::warn("no bsdfs!");
            return;
        }
        auto bsdf_info = info["bsdfs"];
        for (auto &el: bsdf_info.items()) {
            auto bsdf_info = el.value();
            auto bsdf = ProcessBsdf(bsdf_info);

            if (bsdf_info.contains("textures")) {
                auto textures = bsdf_info.at("textures");
                for (auto &tex: textures.items()) {
                    auto texture_info = tex.value();
                    auto texture = ProcessTexture(texture_info);
                    bsdf->AddChild(texture);
                }
            }
            bsdf->Active();

            auto name = bsdf_info.at("id").get<string>();
            bsdf_dic[name] = bsdf;
        }
    }

    shared_ptr<Texture> Parser::ProcessTexture(const json &info) {
        string tex_type = info.at("type").get<string>();
        string name = info.at("name").get<string>();
        if (tex_type == "single_color") {
            Color3f color;
            color = info.at("value").get<vector<float>>();
            return make_shared<SingleColorTexture>(color, name);
        } else if (tex_type == "file") {
            auto filename = info.at("filename").get<string>();
            auto bitmap = img_tool_->PhoenixLoadImage(filename);
            return make_shared<ImageTexture>(bitmap, name);
        }
        return nullptr;


    }


}