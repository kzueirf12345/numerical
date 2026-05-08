#pragma once

#include <cstdint>
#include <vector>

namespace rialto {

struct BatchContext {
    size_t count;
    std::vector<float> start_cost;
    std::vector<float> rate;
    std::vector<float> volatility;
    std::vector<float> strike;
    std::vector<float> period;
};

class Pricer {

public:

    static std::vector<float> BlackScholes(const BatchContext& ctx);

    static std::vector<float> MonteCarlo(
        const BatchContext& ctx, const size_t iterations_cnt, const uint64_t base_seed
    );

};


} //namespace rialto