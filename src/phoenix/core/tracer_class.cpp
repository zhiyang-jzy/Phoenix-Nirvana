#include "phoenix/core/tracer_class.h"
#include <spdlog/spdlog.h>

void ErrorFunction(void *userPtr, enum RTCError error, const char *str) {
    spdlog::error("error: {}", str);
}

namespace Phoenix {
#define FEATURE_MASK \
  RTC_FEATURE_FLAG_TRIANGLE

    Tracer::Tracer() {
        device_ = rtcNewDevice(nullptr);
        if (!device_)
            spdlog::error("error : cannot create device");
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

    TracerHit Tracer::TraceRay(const Ray &ray) const {
        struct RTCRayQueryContext context{};
        rtcInitRayQueryContext(&context);

        struct RTCRayHit ray_hit{};
        ray_hit.ray.org_x = ray.orig.x();
        ray_hit.ray.org_y = ray.orig.y();
        ray_hit.ray.org_z = ray.orig.z();
        ray_hit.ray.dir_x = ray.dir.x();
        ray_hit.ray.dir_y = ray.dir.y();
        ray_hit.ray.dir_z = ray.dir.z();
        ray_hit.ray.tnear = ray.t_near;
        ray_hit.ray.tfar = ray.t_far;
        ray_hit.ray.mask = -1;
        ray_hit.ray.flags = 0;
        ray_hit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        ray_hit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
        RTCIntersectArguments iargs;
        rtcInitIntersectArguments(&iargs);
        iargs.feature_mask = (RTCFeatureFlags) (FEATURE_MASK);

        rtcIntersect1(scene_,  &ray_hit,&iargs);

        return {ray_hit.hit.geomID != RTC_INVALID_GEOMETRY_ID, ray_hit.ray.tfar, {ray_hit.hit.u, ray_hit.hit.v},
                {ray_hit.hit.Ng_x, ray_hit.hit.Ng_y, ray_hit.hit.Ng_z}, ray.At(ray_hit.ray.tfar), ray_hit.hit.geomID,
                ray_hit.hit.primID};

    }

    uint Tracer::AddMesh(const vector<Point3f> &_vertices, const std::vector<uint> &_indices) {
        unsigned int num_vertices = _vertices.size(), num_indices = _indices.size(), num_faces = _indices.size() / 3;
        unsigned int num_edges = num_vertices + num_faces - 2;
        RTCGeometry geom = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);

        auto *vertices = (float *) rtcSetNewGeometryBuffer(geom,
                                                           RTC_BUFFER_TYPE_VERTEX,
                                                           0,
                                                           RTC_FORMAT_FLOAT3,
                                                           3 * sizeof(float),
                                                           num_vertices);
        auto *indices = (unsigned *) rtcSetNewGeometryBuffer(geom,
                                                             RTC_BUFFER_TYPE_INDEX,
                                                             0,
                                                             RTC_FORMAT_UINT3,
                                                             3 * sizeof(unsigned),
                                                             num_indices);

        if (vertices && indices) {
            for (int i = 0; i < _vertices.size(); i++) {
                vertices[i * 3] = _vertices[i].x();
                vertices[i * 3 + 1] = _vertices[i].y();
                vertices[i * 3 + 2] = _vertices[i].z();
            }

            for (int i = 0; i < _indices.size(); i++) {
                indices[i] = _indices[i];
            }
        }

        rtcCommitGeometry(geom);
        unsigned int id = rtcAttachGeometry(scene_, geom);
        rtcReleaseGeometry(geom);
        return id;
    }
}