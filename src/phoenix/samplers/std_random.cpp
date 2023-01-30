#include <random>
#include "phoenix/core/sampler_class.h"

namespace Phoenix{
    class StdRandom : public Sampler{
    private:
        std::random_device device_;
        std::shared_ptr<std::mt19937> gen_;
        std::shared_ptr<std::uniform_real_distribution<>> dis_;

    public:

        StdRandom(const PropertyList& properties){
            gen_ = std::make_shared<std::mt19937>(device_());
            dis_ = std::make_shared<std::uniform_real_distribution<>>(0.0,1.0);
        }
        StdRandom(){
            gen_ = std::make_shared<std::mt19937>(device_());
            dis_ = std::make_shared<std::uniform_real_distribution<>>(0.0,1.0);
        }

        [[nodiscard]] string ToString()const override{ return "independent";}

        float Next1D()override{
            auto &s = *gen_;
            auto &t = *dis_;
            return static_cast<float>(t(s));
        }
        Vector2f Next2D()override{
            auto &s = *gen_;
            auto &t = *dis_;

            auto a1 = t(s),a2=t(s);
            return Vector2f(a1,a2);
        }
        shared_ptr<Sampler> Clone() override{
            return make_shared<StdRandom>();
        }

    };
    PHOENIX_REGISTER_CLASS(StdRandom, "std_random");
}