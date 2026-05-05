#include "rialto/rialto.hpp"

#include <cmath>

namespace rialto {

template <std::floating_point T>
T NormalCDF(T z) {
    return T(0.5) * std::erfc(-z * std::sqrt(T(0.5)));
}

double Rialto::BlackScholes() const {
    const double y1 = (std::log(context_.start_cost / context_.strike) 
                     + (context_.rate + 0.5 * context_.volatility * context_.volatility) * context_.period) 
                     / volatility_mul_sqrt_period_;
    const double y2 = y1 - volatility_mul_sqrt_period_;

    return context_.start_cost * NormalCDF(y1) 
         - context_.strike * std::exp(-context_.rate * context_.period) * NormalCDF(y2);
}

double Rialto::MonteCarlo(const size_t iterations_cnt, const uint64_t seed) {
    gen_.seed(seed);

    double sum_payoffs = 0.;

    const double strike = context_.strike;

    for (size_t iteration = 0; iteration < iterations_cnt; ++iteration) {
        const double stocks_cost = calcStocksCost();

        const double payoff = std::max(stocks_cost - strike, 0.);

        sum_payoffs += payoff;
    }

    double cost = exp_nrate_period_ * (sum_payoffs / iterations_cnt);

    return cost;
}

double Rialto::calcStocksCost() {
    return start_cost_exp_rate_volatility2_div_2_period_ * std::exp(volatility_mul_sqrt_period_ * dist_(gen_));
}

} //namespace rialto