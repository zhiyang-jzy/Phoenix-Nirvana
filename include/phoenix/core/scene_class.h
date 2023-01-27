#pragma once
#include <vector>
#include <map>
#include "object_class.h"
#include "tracer_class.h"
#include "common_struct.h"
namespace Phoenix{
    class Shape;
    class Emitter;
    class Scene : public PhoenixObject{
    private:
        Tracer tracer_;
        std::vector<shared_ptr<Shape>> shapes_;
        std::vector<shared_ptr<Emitter>> emitters_;
        std::map<uint,shared_ptr<Shape>> shape_dict_;
        std::map<uint,shared_ptr<Emitter>> emitter_dict_;

    public:
        Scene()=default;
        PClassType GetClassType()const override{
            return PClassType::PScene;
        }
        string ToString()const override{
            return "scene";
        }
        void AddShape(const shared_ptr<Shape>& shape);
        void AddEmitter(const shared_ptr<Emitter> emitter);
        void SetDict(uint id,shared_ptr<Shape> shape);
        void SetDict(uint id,shared_ptr<Emitter> emitter);
        Tracer& tracer(){return tracer_;}
        void FinishAdd(){tracer_.FinishAdd();}
        shared_ptr<Emitter> SampleEmitter(float& pdf,Vector2f sample);

        Interaction Trace(const Ray &ray);

    };
}