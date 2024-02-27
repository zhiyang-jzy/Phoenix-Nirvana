#pragma once
#include <embree4/rtcore.h>
#include "phoenix/core/common.h"
#include "phoenix/core/common_struct.h"

#if defined(RTC_NAMESPACE_USE)
RTC_NAMESPACE_USE
#endif

namespace Phoenix{
    class Tracer{
    private:
        RTCDevice device_;
        RTCScene scene_;
    public:
        Tracer();
        uint AddSphere(const Vector3f& center,float radius);
        uint AddMesh(const std::vector<Point3f> &_vertices, const std::vector<uint>& indices);
        TracerHit TraceRay (const Ray& ray)const ;
        void FinishAdd();
    };

}