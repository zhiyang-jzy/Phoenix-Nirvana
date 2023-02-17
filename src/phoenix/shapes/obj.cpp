#include "phoenix/core/shape_class.h"
#include "phoenix/utils/model_loader_class.h"

namespace Phoenix {
    class ObjShape : public Shape {
    private:
        string file_name_;
        shared_ptr<Model> model_;
        map<uint, shared_ptr<Mesh>> mesh_dict_;
    public:
        ObjShape(PropertyList property_list) {
            file_name_ = property_list.Get<string>("filename").value();
            ModelLoader m_loader;
            model_ = m_loader.Load(file_name_);
            area_ = model_->area_;
            inv_area_ = 1.f / area_;
        }

        string ToString() const override { return "obj"; }

        vector<uint> AddToTracer(Tracer &tracer) override {
            vector<uint> ids;
            std::for_each(model_->meshes_.begin(), model_->meshes_.end(), [&](auto mesh) {
                uint id = tracer.AddMesh(mesh->vertices_, mesh->indices_);
                mesh_dict_[id] = mesh;
                ids.push_back(id);
            });
            return ids;
        }

        void SamplePosition(PositionSampleRecord &rec, Vector2f sample) override {
            model_->SamplePosition(rec, sample);
        }

        void ApplyTransform(const Transform &trans) override {
            model_->ApplyTransform(trans);
            area_ = model_->area_;
            inv_area_ = 1.f / area_;
        }

        Vector2f GetUv(const Interaction &its) const override {
            auto mesh = mesh_dict_.at(its.basic.geo_id);
            return mesh->GetUv(its.basic.prim_id, its.basic.uv);

        }

        float PdfPosition(const PositionSampleRecord &pRec) const override {
            return inv_area_;

        }

        Normal3f GetNormal(const Interaction &its) const override {
            auto mesh = mesh_dict_.at(its.basic.geo_id);
            auto normal = mesh->GetNormal(its.basic.prim_id, its.basic.uv);
            if (normal)
                return normal.value();
            return its.basic.normal;
        }


    };

    PHOENIX_REGISTER_CLASS(ObjShape, "obj");
}