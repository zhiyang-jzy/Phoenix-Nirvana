#include "phoenix/core/model_class.h"
#include <spdlog/spdlog.h>

#include <utility>
namespace Phoenix {

    Mesh::Mesh(vector<Vertex> vertices, vector<uint> indices, shared_ptr<Texture> texture) : vertices_(std::move(vertices)),
                                                                                             indices_(std::move(indices)),
                                                                                             texture_(std::move(texture)) {
        PostProcess();
    }

    void Mesh::PostProcess() {
        area_ = 0;
        dpdf_.Clear();
        auto vertex_count = indices_.size();
        for (int i = 0; i < vertex_count; i += 3) {
            auto a = vertices_[indices_[i]].position, b = vertices_[indices_[i + 1]].position,
                    c = vertices_[indices_[i + 2]].position;
            float area = (c - a).cross(b - a).norm() * 0.5f;
            area_ += area;
            dpdf_.Append(area);
        }
        spdlog::info("calc area {}", area_);
        dpdf_.Normalize();

    }

    Model::Model() {

    }

    void Model::AddMesh(const shared_ptr<Mesh>& mesh) {
        meshes_.push_back(mesh);
    }

    void Model::PostProcess() {
        area_ = 0.f;
        dpdf_.Clear();
        std::for_each(meshes_.begin(), meshes_.end(), [&](const auto &item) {
            auto area = item->area();
            area_+=area;
            dpdf_.Append(area);
        });
        dpdf_.Normalize();
    }
}