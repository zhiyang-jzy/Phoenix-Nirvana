#pragma once
#include "phoenix/core/common_struct.h"
namespace Phoenix

{
    class MicrofacetDistribution{
    private:
        float alpha_;
    private:
        Vector2f Sample11(float theta,Vector2f sample) const;
        Normal3f SampleVisible(const Vector3f &_wi, const Vector2f &sample) const ;

        float ProjectRoughness(const Vector3f& v)const;
    public:
        MicrofacetDistribution(float alpha):alpha_(alpha){};
        float Eval(const Vector3f& m);
        Normal3f Sample(const Vector3f& wi_, const Vector2f& sample, float &pdf);
        float Pdf(const Vector3f& wi,const Normal3f& m);
        float G(const Vector3f& wi,const Vector3f& wo,const Normal3f& m)const;
        float SmithG1(const Vector3f& v,const Vector3f& m)const;

    };
}
