#include "phoenix/core/shape_class.h"

namespace Phoenix {
    void Shape::SampleDirect(Phoenix::DirectSamplingRecord &dRec, const Phoenix::Vector2f &sample) {
        SamplePosition(dRec, sample);

        dRec.dir = dRec.point - dRec.ref;

        float distSquared = dRec.dir.squaredNorm();
        dRec.dist = std::sqrt(distSquared);
        dRec.dir /= dRec.dist;
        float dp = abs(dRec.dir.dot(dRec.normal));
        dRec.pdf = inv_area_;
        dRec.pdf *= dp != 0 ? (distSquared / dp) : 0.0f;

    }

    float Shape::PdfDirect(const DirectSamplingRecord &dRec) {
        float pdfPos = PdfPosition(dRec);
        return pdfPos * (dRec.dist * dRec.dist) / abs(dRec.dir.dot(dRec.normal));
    }
}
