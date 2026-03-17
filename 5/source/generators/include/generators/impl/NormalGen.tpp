#pragma once

#include <numbers>

#include "../NormalGen.hpp"

template <std::floating_point T>
NormalGen<T>::NormalGen(BaseRngT<T> base_rng) 
    :   base_rng_(base_rng)
{}

template <std::floating_point T>
T NormalGen<T>::operator()(T mean, T stddev) {
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