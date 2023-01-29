#pragma once
#include <embree3/rtcore.h>
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
        TracerHit TraceRay (const Ray& ray)const ;
        void FinishAdd();
    };

}