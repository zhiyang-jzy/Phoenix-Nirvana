#include "phoenix/core/model_class.h"
#include "phoenix/core/shape_class.h"
#include <spdlog/spdlog.h>

#include <utility>

namespace Phoenix {

    Mesh::Mesh(vector<Vertex> vertices, vector<uint> indices, map<TextureType, shared_ptr<Texture> > textures)
            : vertexes_(std::move(vertices)),
              indices_(std::move(indices)),
              textures_(std::move(textures)) {
        PostProcess();

    }

    void Mesh::PostProcess() {
        vertices_.clear();
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

    void Mesh::SamplePosition(PositionSampleRecord &pos_rec, Vector2f sample) {
        float pdf;
        auto index = dpdf_.Sample(sample.x(), pdf);
        index *= 3;

        float alpha = 1 - sqrt(1 - sample.x());
        float beta = sample.y() * sqrt(1 - sample.x());
        float theta = 1 - alpha - beta;

        Point3f a(vertices_[indices_[index]]), b(vertices_[indices_[index + 1]]),
                c(vertices_[indices_[index + 2]]);
        Normal3f na = vertexes_[indices_[index]].normal, nb = vertexes_[indices_[index +
                                                                                 1]].normal, nc = vertexes_[indices_[
                index + 2]].normal;

        pos_rec.point = a * alpha + b * beta + c * theta;
        pos_rec.pdf = pdf;
        pos_rec.normal = (na * alpha + nb * beta + nc * theta).normalized();

    }

    void Mesh::ApplyTransform(const Transform &trans) {
        dpdf_.Clear();
        vertices_.clear();
        for (auto &vertex: vertexes_) {
            vertex.position = trans * vertex.position;
            vertex.normal = trans * vertex.normal;
            vertex.tangent = trans * vertex.tangent;
            vertex.b_tangent = trans * vertex.b_tangent;
        }
        PostProcess();


    }

    Vector2f Mesh::GetUv(uint prim_id, Vector2f uv) {
        if (prim_id * 3 + 2 >= indices_.size()) {
            return {0, 0};
        }
        auto ind1 = indices_[prim_id * 3], ind2 = indices_[prim_id * 3 + 1], ind3 = indices_[prim_id * 3 + 2];
        auto coord1 = vertexes_[ind1].texcoord, coord2 = vertexes_[ind2].texcoord, coord3 = vertexes_[ind3].texcoord;
        return (1 - uv.x() - uv.y()) * coord1 + uv.x() * coord2 + uv.y() * coord3;
    }

    Model::Model() {
        dpdf_.Clear();

    }

    void Model::AddMesh(const shared_ptr<Mesh> &mesh) {
        meshes_.push_back(mesh);
    }

    void Model::PostProcess() {
        area_ = 0.f;
        dpdf_.Clear();
        std::for_each(meshes_.begin(), meshes_.end(), [&](const auto &item) {
            auto area = item->area();
            area_ += area;
            dpdf_.Append(area);
        });
        dpdf_.Normalize();
    }

    void Model::SamplePosition(PositionSampleRecord &pos_rec, Vector2f sample) {
        float mesh_pdf;
        auto mesh_index = dpdf_.Sample(sample.x(), mesh_pdf);
        auto mesh = meshes_[mesh_index];
        mesh->SamplePosition(pos_rec, sample);
        pos_rec.pdf *= mesh_pdf;
    }

    void Model::ApplyTransform(const Transform &trans) {
        dpdf_.Clear();
        for (auto &mesh: meshes_) {
            mesh->ApplyTransform(trans);
        }
        PostProcess();
    }
}