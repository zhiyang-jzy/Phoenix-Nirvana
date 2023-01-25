#pragma once
#include "object_class.h"
#include "tracer_class.h"
namespace Phoenix{
    class Scene : public PhoenixObject{
    private:
        Tracer tracer;

    public:
        PClassType GetClassType()const override{
            return PClassType::PScene;
        }
        string ToString()const override{
            return "scene";
        }
        void AddSphere()



    };
}