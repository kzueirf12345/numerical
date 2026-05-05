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
        ,   start_cost_exp_rate_volatility2_div_2_period_(
            context_.start_cost * std::exp(
                (context_.rate - context_.volatility * context_.volatility / 2) * context_.period 
            )
        )
        ,   volatility_mul_sqrt_period_(context_.volatility * std::sqrt(context_.period))
        ,   exp_nrate_period_(std::exp(-context_.rate * context_.period))
    {}

    double MonteCarlo(const size_t iterations_cnt, const uint64_t seed);

    double BlackScholes() const;

private:

    double calcStocksCost();

private:

    Context context_;
    const double start_cost_exp_rate_volatility2_div_2_period_;
    const double volatility_mul_sqrt_period_;
    const double exp_nrate_period_;

private:

    std::mt19937 gen_;
    std::normal_distribution<double> dist_{0., 1.};
};

} //namespace rialto