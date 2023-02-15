#include "phoenix/core/discrete_pdf_class.h"

namespace Phoenix {

    void DiscretePdf::Append(float pdf_value) {
        cdf_.push_back(pdf_value + cdf_[cdf_.size()-1]);
    }

    void DiscretePdf::Clear() {
        cdf_.clear();
        cdf_.push_back(0.f);
        sum_ = 0.f;
    }

    uint DiscretePdf::Sample(float sample_value, float &pdf) const {
        auto entry =
                std::lower_bound(cdf_.begin(), cdf_.end(), sample_value);
        uint index = (uint)(std::max((ptrdiff_t)0,entry-cdf_.begin()-1));
        pdf = operator[](index);
        return std::min(index,(uint)cdf_.size()-1);
    }

    float DiscretePdf::operator[](uint index)const {
        return cdf_[index + 1] - cdf_[index];
    }

    void DiscretePdf::Normalize() {
        sum_ = cdf_[cdf_.size()-1];
        if (sum_ > 0) {
            normalization_ = 1.0f / sum_;
            for (size_t i=1; i<cdf_.size(); ++i)
                cdf_[i] *= normalization_;
            cdf_[cdf_.size()-1] = 1.0f;
            normalized_ = true;
        } else {
            normalization_ = 0.0f;
        }
    }

    uint DiscretePdf::sampleReuse(float &sampleValue) const {
        float pdf;
        size_t index = Sample(sampleValue,pdf);
        sampleValue = (sampleValue - cdf_[index])
                      / (cdf_[index + 1] - cdf_[index]);
        return index;
    }

    uint DiscretePdf::SampleReuse(float &sampleValue, float &pdf) const {
        size_t index = Sample(sampleValue, pdf);
        sampleValue = (sampleValue - cdf_[index])
                      / (cdf_[index + 1] - cdf_[index]);
        return index;
    }
}