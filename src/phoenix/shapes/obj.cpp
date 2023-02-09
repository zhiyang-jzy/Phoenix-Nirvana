#include "phoenix/core/shape_class.h"
#include "phoenix/utils/model_loader_class.h"

namespace Phoenix {
    class ObjShape : public Shape {
    private:
        string file_name_;
        shared_ptr<Model> model_;
    public:
        ObjShape(PropertyList property_list) {
            file_name_ = property_list.Get<string>("filename").value();
            ModelLoader m_loader;
            model_ = m_loader.Load(file_name_);
            area_ = model_->area_;

        }

        string ToString() const override { return "obj"; }

        vector<uint> AddToTracer(Tracer &tracer) override {
            vector<uint> ids;
            std::for_each(model_->meshes_.begin(), model_->meshes_.end(), [&](const auto &mesh) {
                uint id = tracer.AddMesh(mesh->vertices_,mesh->indices_);
                ids.push_back(id);
            });
            return ids;
        }

        void SamplePosition(PositionSampleRecord &rec, Vector2f sample) override {
            model_->SamplePosition(rec,sample);
        }

        void ApplyTransform(const Transform &trans) override{
            model_->ApplyTransform(trans);
        }
    };

    PHOENIX_REGISTER_CLASS(ObjShape, "obj");
}