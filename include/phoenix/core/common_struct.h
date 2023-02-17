#pragma once

#include <utility>

#include "common.h"
#include "vector_class.h"

namespace Phoenix {
    class Shape;

    class Emitter;

    class Frame;

    enum class HitType {
        Shape, Emitter
    };

    struct Ray {
        Vector3f orig;
        Vector3f dir;
        float t_near;
        float t_far;
        Ray()=default;
        Ray(Vector3f ori,Vector3f di);
        Vector3f At(float t) const;
    };


    struct Transform {
    public:

        Transform() :
                m_transform(Eigen::Matrix4f::Identity()),
                m_inverse(Eigen::Matrix4f::Identity()) { }


        explicit Transform(const Eigen::Matrix4f &trafo):m_transform(trafo),m_inverse(trafo.inverse()){}
        explicit Transform(vector<float>& vector);


        Transform(Eigen::Matrix4f trafo, Eigen::Matrix4f inv)
                : m_transform(std::move(trafo)), m_inverse(std::move(inv)) { }


        [[nodiscard]] const Eigen::Matrix4f &getMatrix() const {
            return m_transform;
        }


        [[nodiscard]] const Eigen::Matrix4f &getInverseMatrix() const {
            return m_inverse;
        }


        [[nodiscard]] Transform inverse() const {
            return {m_inverse, m_transform};
        }
        Transform operator*(const Transform &t) const;


        Vector3f operator*(const Vector3f &v) const {
            return m_transform.topLeftCorner<3,3>() * v;
        }


        Normal3f operator*(const Normal3f &n) const {
            return m_inverse.topLeftCorner<3, 3>().transpose() * n;
        }


        Point3f operator*(const Point3f &p) const {
            Vector4f result = m_transform * Vector4f(p[0], p[1], p[2], 1.0f);
            return result.head<3>() / result.w();
        }



        std::string toString() const;
    private:
        Eigen::Matrix4f m_transform;
        Eigen::Matrix4f m_inverse;
    };



    inline void CoordinateSystem(const Vector3f &a, Vector3f &b, Vector3f &c) {
        if (std::abs(a.x()) > std::abs(a.y())) {
            float invLen = 1.0f / std::sqrt(a.x() * a.x() + a.z() * a.z());
            c = Vector3f(a.z() * invLen, 0.0f, -a.x() * invLen);
        } else {
            float invLen = 1.0f / std::sqrt(a.y() * a.y() + a.z() * a.z());
            c = Vector3f(0.0f, a.z() * invLen, -a.y() * invLen);
        }
        b = c.cross(a);
    }

    class Frame {
    public:
        Vector3f s, t;
        Vector3f n;
    public:
        explicit Frame(Vector3f n) : n(n) {
            CoordinateSystem(n, s, t);
        }

        Frame() = default;

        Vector3f ToLocal(const Vector3f &v) const {
            return Vector3f(
                    v.dot(s), v.dot(t), v.dot(n)
            );
        }

        Vector3f ToWorld(const Vector3f &v) const {
            return s * v.x() + t * v.y() + n * v.z();
        }

        static float CosTheta(const Vector3f &v) {
            return v.z();
        }
        static float cosTheta2(const Vector3f &v) {
            return v.z() * v.z();
        }

        static float SinTheta(const Vector3f &v) {
            float temp = SinTheta2(v);
            if (temp <= 0.0f)
                return 0.0f;
            return std::sqrt(temp);
        }

        static float TanTheta(const Vector3f &v) {
            float temp = 1 - v.z() * v.z();
            if (temp <= 0.0f)
                return 0.0f;
            return std::sqrt(temp) / v.z();
        }

        static float SinTheta2(const Vector3f &v) {
            return 1.0f - v.z() * v.z();
        }


        static float SinPhi(const Vector3f &v) {
            float sinTheta = Frame::SinTheta(v);
            if (sinTheta == 0.0f)
                return 1.0f;
            return Clamp(v.y() / sinTheta, -1.0f, 1.0f);
        }

        static float CosPhi(const Vector3f &v) {
            float sinTheta = Frame::SinTheta(v);
            if (sinTheta == 0.0f)
                return 1.0f;
            return Clamp(v.x() / sinTheta, -1.0f, 1.0f);
        }

        static float sinPhi2(const Vector3f &v) {
            return Clamp(v.y() * v.y() / SinTheta2(v), 0.0f, 1.0f);
        }


        static float cosPhi2(const Vector3f &v) {
            return Clamp(v.x() * v.x() / SinTheta2(v), 0.0f, 1.0f);
        }

    };

    struct TracerHit {
        bool is_hit;
        float t_far;
        Vector2f uv;
        Normal3f normal;
        Point3f point;
        uint geo_id;
        uint prim_id;
    };
    struct Interaction {
        TracerHit basic;
        shared_ptr<Shape> shape;
        shared_ptr<Emitter> emitter;
        HitType hit_type;
        Frame frame;
        Vector2f uv;
        Normal3f normal;
    };

    struct PositionSampleRecord {
        Vector3f point;
        Normal3f normal;
        float pdf;
    };

    struct DirectSamplingRecord : public PositionSampleRecord{
        Point3f ref;
        Vector3f dir;
        float dist;
        shared_ptr<Emitter> emitter;

        DirectSamplingRecord(Point3f ref_): ref(ref_){}

        void SetQuery(const Ray& ray,const Interaction& its){
            point = its.basic.point;
            normal = its.normal;
            emitter = its.emitter;
            dir = ray.dir;
            dist = (ray.orig-point).norm();
        }

    };


}