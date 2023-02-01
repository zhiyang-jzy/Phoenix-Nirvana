#include "phoenix/core/model_class.h"
#include <spdlog/spdlog.h>

#include <utility>
namespace Phoenix {

    Mesh::Mesh(vector<Vertex> vertices, vector<uint> indices, map<TextureType,shared_ptr<Texture> > textures) : vertexes_(std::move(vertices)),
                                                                                             indices_(std::move(indices)),
                                                                                             textures_(std::move(textures)) {
        PostProcess();

    }

    void Mesh::PostProcess() {

        std::for_each(vertexes_.begin(), vertexes_.end(), [&](const auto &item) {
            vertices_.push_back(item.position);
        });

        area_ = 0;
        dpdf_.Clear();
        auto vertex_count = indices_.size();
        for (int i = 0; i < vertex_count; i += 3) {
            auto a = vertexes_[indices_[i]].position, b = vertexes_[indices_[i + 1]].position,
                    c = vertexes_[indices_[i + 2]].position;
            float area = (c - a).cross(b - a).norm() * 0.5f;
            area_ += area;
            dpdf_.Append(area);
        }
        spdlog::info("calc area {}", area_);
        dpdf_.Normalize();

    }

    Model::Model() {
        dpdf_.Clear();

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

    void Model::SamplePosition(PositionSampleRecord &pos_rec, Vector2f sample) {
        float mesh_pdf;
        auto index = dpdf_.Sample(sample.x(),mesh_pdf);
    }
}