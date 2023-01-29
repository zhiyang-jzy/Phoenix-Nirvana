#include "phoenix/core/sampler_class.h"
#include "ext/pcg32.h"

namespace Phoenix {

    class PcgRandom : public Sampler {
    public:
        pcg32 random_;
        std:: mutex m;

    public:
        explicit PcgRandom(const PropertyList &props) {

        }

        float Next1D() override {
            //std::scoped_lock lock(m);
            return random_.nextFloat();
        }

        void Prepare(const ImageBlock &block) override {
//            random_.seed(
//                    block.offset().x(),
//                    block.offset().y()
//            );
        }

        PcgRandom() = default;

        [[nodiscard]] shared_ptr<Sampler> Clone() override {
            auto cloned = make_shared<PcgRandom>();
            cloned->random_ = random_;
            return std::move(cloned);
        }

        Vector2f Next2D() override {
            //std::scoped_lock lock(m);
            return {random_.nextFloat(), random_.nextFloat()};
        }
        string ToString()const override{
            return "pcg_random";
        }

    };

    PHOENIX_REGISTER_CLASS(PcgRandom, "pcg_random");

}

