#pragma once

#include <concepts>
#include <cmath>
#include <numbers>

#include "common.hpp"


template <std::floating_point T>
class NormalGen {

public:

    NormalGen(BaseRngT<T> base_rng) 
        :   base_rng_(base_rng)
    {}

    T operator()(T mean = 0., T stddev = 1.) {
        if (is_cached_) {
            is_cached_ = false;
            return mean + stddev * cache_val_;
        }
        T u1, u2, first_mul, trig_arg;

        do {
            u1 = base_rng_();
        } while(u1 == 0.);

        do {
            u2 = base_rng_();
        } while(u2 == 0.);

        first_mul = static_cast<T>(std::sqrt(-2 * std::log(u1)));
        trig_arg = 2 * std::numbers::pi * u2;

        T z1       = first_mul * std::cos(trig_arg);
        cache_val_ = first_mul * std::sin(trig_arg);

        is_cached_ = true;

        return mean + stddev * z1;
    }

private:

    BaseRngT<T> base_rng_;

    bool is_cached_ = false;
    double cache_val_ = NAN;
};