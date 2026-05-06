#pragma once

#include <cstdint>
#include <random>

namespace rialto {

class Rialto {

public:

    struct Context {
        double start_cost = 100;
        double rate = 0.05;
        double volatility = 0.1;
        double strike = 100;
        double period = 1;
    };

    Rialto(Context context)
        :   context_(context)
        ,   lognorm_m_(std::log(context.start_cost) + 
                      (context.rate - 0.5 * context.volatility * context.volatility) * context.period)
        ,   lognorm_s_(context.volatility * std::sqrt(context.period))
        ,   exp_nrate_period_(std::exp(-context_.rate * context_.period))
        ,   dist_(lognorm_m_, lognorm_s_)
    {}

    double MonteCarlo(const size_t iterations_cnt, const uint64_t seed);

    double BlackScholes() const;

private:

    double calcStocksCost();

private:

    Context context_;
    const double lognorm_m_;
    const double lognorm_s_;
    const double exp_nrate_period_;

private:

    std::mt19937 gen_;
    std::lognormal_distribution<double> dist_;
};

} //namespace rialto