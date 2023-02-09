#pragma once
#include <Eigen/Eigen>

namespace Phoenix {
    template<typename _Scalar, int _Dimension>
    struct TVector : public Eigen::Matrix<_Scalar, _Dimension, 1> {
    public:
        enum {
            Dimension = _Dimension
        };

        typedef _Scalar Scalar;
        typedef Eigen::Matrix<Scalar, Dimension, 1> Base;


        explicit TVector(Scalar value = (Scalar) 0) { Base::setConstant(value); }


        TVector(Scalar x, Scalar y) : Base(x, y) {}


        TVector(Scalar x, Scalar y, Scalar z) : Base(x, y, z) {}


        TVector(Scalar x, Scalar y, Scalar z, Scalar w) : Base(x, y, z, w) {}


        template<typename Derived>
        TVector(const Eigen::MatrixBase <Derived> &p)
                : Base(p) {}


        template<typename Derived>
        TVector &operator=(const Eigen::MatrixBase <Derived> &p) {
            this->Base::operator=(p);
            return *this;
        }
        TVector &operator=(const std::vector<float> &p) {
            for(int i=0;i<Dimension;i++)
                this->coeffRef(i) = p[i];
            return *this;
        }


        std::string toString() const {
            std::string result;
            for (size_t i = 0; i < Dimension; ++i) {
                result += std::to_string(this->coeff(i));
                if (i + 1 < Dimension)
                    result += ", ";
            }
            return "[" + result + "]";
        }
    };

    template<typename _Scalar, int _Dimension>
    struct TPoint : public Eigen::Matrix<_Scalar, _Dimension, 1> {
    public:
        enum {
            Dimension = _Dimension
        };

        typedef _Scalar Scalar;
        typedef Eigen::Matrix<Scalar, Dimension, 1> Base;
//        typedef TVector<Scalar, Dimension> VectorType;
//        typedef TPoint<Scalar, Dimension> PointType;


        TPoint(Scalar value = (Scalar) 0) { Base::setConstant(value); }


        TPoint(Scalar x, Scalar y) : Base(x, y) {}


        TPoint(Scalar x, Scalar y, Scalar z) : Base(x, y, z) {}


        TPoint(Scalar x, Scalar y, Scalar z, Scalar w) : Base(x, y, z, w) {}


        template<typename Derived>
        TPoint(const Eigen::MatrixBase <Derived> &p)
                : Base(p) {}


        template<typename Derived>
        TPoint &operator=(const Eigen::MatrixBase <Derived> &p) {
            this->Base::operator=(p);
            return *this;
        }
        TPoint &operator=(const std::vector<float> &p) {
            for(int i=0;i<Dimension;i++)
                this->coeffRef(i) = p[i];
            return *this;
        }


        std::string toString() const {
            std::string result;
            for (size_t i = 0; i < Dimension; ++i) {
                result += std::to_string(this->coeff(i));
                if (i + 1 < Dimension)
                    result += ", ";
            }
            return "[" + result + "]";
        }
    };

    struct Normal3f : public Eigen::Matrix<float, 3, 1> {
    public:
        enum {
            Dimension = 3
        };

        typedef float Scalar;
        typedef Eigen::Matrix<Scalar, Dimension, 1> Base;
        typedef TVector<Scalar, Dimension> VectorType;
        typedef TPoint<Scalar, Dimension> PointType;


        Normal3f(Scalar value = 0.0f) { Base::setConstant(value); }


        Normal3f(Scalar x, Scalar y, Scalar z) : Base(x, y, z) {}


        template<typename Derived>
        Normal3f(const Eigen::MatrixBase <Derived> &p)
                : Base(p) {}


        template<typename Derived>
        Normal3f &operator=(const Eigen::MatrixBase <Derived> &p) {
            this->Base::operator=(p);
            return *this;
        }


    };

    struct Color3f : public Eigen::Array3f {
    public:
        typedef Eigen::Array3f Base;


        Color3f(float value = 0.f) : Base(value, value, value) { }


        Color3f(float r, float g, float b) : Base(r, g, b) { }


        template <typename Derived> Color3f(const Eigen::ArrayBase<Derived>& p)
                : Base(p) { }


        template <typename Derived> Color3f &operator=(const Eigen::ArrayBase<Derived>& p) {
            this->Base::operator=(p);
            return *this;
        }
        Color3f &operator=(const std::vector<float> &p) {
            for(int i=0;i<3;i++)
                this->coeffRef(i) = p[i];
            return *this;
        }

        template<typename Derived>
        Color3f &operator=(const Eigen::MatrixBase <Derived> &p) {
            this->Base::operator=(p);
            return *this;
        }


        float &r() { return x(); }

        const float &r() const { return x(); }

        float &g() { return y(); }

        const float &g() const { return y(); }

        float &b() { return z(); }

        const float &b() const { return z(); }


        Color3f clamp() const { return Color3f(std::max(r(), 0.0f),
                                               std::max(g(), 0.0f), std::max(b(), 0.0f)); }


        bool isValid() const{
            for (int i=0; i<3; ++i) {
                float value = coeff(i);
                if (value < 0 || !std::isfinite(value))
                    return false;
            }
            return true;
        }


        Color3f toLinearRGB() const;


        Color3f toSRGB() const;


        float getLuminance() const;


    };

/**
 * \brief Represents a linear RGB color and a weight
 *
 * This is used by Nori's image reconstruction filter code
 */
    struct Color4f : public Eigen::Array4f {
    public:
        typedef Eigen::Array4f Base;


        Color4f() : Base(0.0f, 0.0f, 0.0f, 0.0f) { }


        Color4f(const Color3f &c) : Base(c.r(), c.g(), c.b(), 1.0f) { }


        Color4f(float r, float g, float b, float w) : Base(r, g, b, w) { }


        template <typename Derived> Color4f(const Eigen::ArrayBase<Derived>& p)
                : Base(p) { }


        template <typename Derived> Color4f &operator=(const Eigen::ArrayBase<Derived>& p) {
            this->Base::operator=(p);
            return *this;
        }


        Color3f divideByFilterWeight() const {
            if (w() != 0)
                return head<3>() / w();
            else
                return Color3f(0.0f);
        }


    };

    typedef TVector<float, 1>       Vector1f;
    typedef TVector<float, 2>       Vector2f;
    typedef TVector<float, 3>       Vector3f;
    typedef TVector<float, 4>       Vector4f;
    typedef TVector<double, 1>      Vector1d;
    typedef TVector<double, 2>      Vector2d;
    typedef TVector<double, 3>      Vector3d;
    typedef TVector<double, 4>      Vector4d;
    typedef TVector<int, 1>         Vector1i;
    typedef TVector<int, 2>         Vector2i;
    typedef TVector<int, 3>         Vector3i;
    typedef TVector<int, 4>         Vector4i;
    typedef TPoint<float, 1>        Point1f;
    typedef TPoint<float, 2>        Point2f;
    typedef TPoint<float, 3>        Point3f;
    typedef TPoint<float, 4>        Point4f;
    typedef TPoint<double, 1>       Point1d;
    typedef TPoint<double, 2>       Point2d;
    typedef TPoint<double, 3>       Point3d;
    typedef TPoint<double, 4>       Point4d;
    typedef TPoint<int, 1>          Point1i;
    typedef TPoint<int, 2>          Point2i;

}