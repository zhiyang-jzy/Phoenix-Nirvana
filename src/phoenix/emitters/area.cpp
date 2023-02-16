#include "phoenix/core/emitter_class.h"

namespace Phoenix {
    class AreaEmitter : public Emitter {
    private:
        shared_ptr<Shape> shape_;
    public:
        explicit AreaEmitter(PropertyList properties) {
            radiance_ = properties.Get<vector<float>>("radiance").value_or(vector<float>{1, 1, 1});
        }

        float area() override {
            return shape_->area();
        }

        [[nodiscard]] string ToString() const override { return "area light"; }

        void AddChild(shared_ptr<PhoenixObject> child) override {
            shape_ = std::dynamic_pointer_cast<Shape>(child);
        }

        [[nodiscard]] Color3f Eval(const EmitterQueryRecord &rec) const override {
//            float v = rec.n.dot(rec.wi);
//            if(v<=0.f)
//                return {0,0,0};
            return radiance_;
        }

        Color3f Sample(EmitterQueryRecord &rec, Vector2f sample) const override {
            PositionSampleRecord pos_res;
            shape_->SamplePosition(pos_res, sample);

            rec.n = pos_res.normal.normalized();
            rec.p = pos_res.point;
            rec.pdf = pos_res.pdf;
            rec.wi = (rec.ref - rec.p).normalized();
            rec.shadow_ray = Ray(rec.ref, -rec.wi);

            if (rec.n.dot(rec.wi) <= 0)
//                return {0,0,0};
                rec.n = -rec.n;
            return radiance_;

        }

        void AddToScene(Scene &scene) override {
            auto ids = shape_->AddToTracer(scene.tracer());
            std::for_each(ids.begin(), ids.end(), [&](const auto &item) {
                scene.SetDict(item, shared_ptr<Emitter>(this));
            });
        }

        Color3f SampleDirect(DirectSamplingRecord &dRec, const Vector2f &sample) const override {
            shape_->SampleDirect(dRec, sample);
            if (dRec.pdf != 0) {
                return radiance_ / dRec.pdf;
            } else {
                dRec.pdf = 0.0f;
                return Color3f(0.f);
            }
        }

        float PdfPosition(const PositionSampleRecord &pRec) const override {
            return shape_->PdfPosition(pRec);
        }

        float PdfDirect(const DirectSamplingRecord &dRec) const override{
            return shape_->PdfDirect(dRec);
        }


    };

    PHOENIX_REGISTER_CLASS(AreaEmitter, "area");
}