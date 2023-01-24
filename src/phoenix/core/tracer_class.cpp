#include "phoenix/core/tracer_class.h"
#include <spdlog/spdlog.h>

void ErrorFunction(void *userPtr, enum RTCError error, const char *str) {
    spdlog::error("error {}: {}", error, str);
}

namespace Phoenix {

    Tracer::Tracer() {
        device_ = rtcNewDevice(nullptr);
        if (!device_)
            spdlog::error("error {}: cannot create device", rtcGetDeviceError(nullptr));
        rtcSetDeviceErrorFunction(device_, ErrorFunction, nullptr);

        scene_ = rtcNewScene(device_);
    }

    uint Tracer::AddSphere(const Vector3f &center, float radius) {
        RTCGeometry geom = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_SPHERE_POINT);

        auto *vertices = (float *) rtcSetNewGeometryBuffer(geom,
                                                           RTC_BUFFER_TYPE_VERTEX,
                                                           0,
                                                           RTC_FORMAT_FLOAT4,
                                                           4 * sizeof(float),
                                                           1);


        vertices[0] = static_cast<float>(center.x());
        vertices[1] = static_cast<float>(center.y());
        vertices[2] = static_cast<float>(center.z());
        vertices[3] = static_cast<float>(radius);

        rtcCommitGeometry(geom);
        unsigned int id = rtcAttachGeometry(scene_, geom);
        rtcReleaseGeometry(geom);

        return id;
    }

    void Tracer::FinishAdd() {
        rtcCommitScene(scene_);
    }

    RTCRayHit Tracer::TraceRay(const Ray &ray) {
        struct RTCIntersectContext context{};
        rtcInitIntersectContext(&context);

        struct RTCRayHit rayhit{};
        rayhit.ray.org_x = ray.orig.x();
        rayhit.ray.org_y = ray.orig.y();
        rayhit.ray.org_z = ray.orig.z();
        rayhit.ray.dir_x = ray.dir.x();
        rayhit.ray.dir_y = ray.dir.y();
        rayhit.ray.dir_z = ray.dir.z();
        rayhit.ray.tnear = ray.t_near;
        rayhit.ray.tfar = ray.t_far;
        rayhit.ray.mask = -1;
        rayhit.ray.flags = 0;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

        rtcIntersect1(scene_, &context, &rayhit);

        return rayhit;
    }
}