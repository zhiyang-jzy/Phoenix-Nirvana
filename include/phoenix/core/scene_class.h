#pragma once
#include <vector>
#include <map>
#include "object_class.h"
#include "tracer_class.h"
#include "common_struct.h"
namespace Phoenix{
    class Shape;
    class Scene : public PhoenixObject{
    private:
        Tracer tracer_;
        std::vector<shared_ptr<Shape>> shapes_;
        std::map<uint,shared_ptr<Shape>> shape_dict_;

    public:
        PClassType GetClassType()const override{
            return PClassType::PScene;
        }
        string ToString()const override{
            return "scene";
        }
        void AddShape(const shared_ptr<Shape>& shape);
        void SetDict(uint id,shared_ptr<Shape> shape);
        Tracer& tracer(){return tracer_;}
        void FinishAdd(){tracer_.FinishAdd();}

        Interaction Trace(const Ray &ray);

    };
}