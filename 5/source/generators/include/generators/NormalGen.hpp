#pragma once

#include <concepts>
#include <cmath>

#include "generators/common.hpp"


template <std::floating_point T>
class NormalGen {

public:

    NormalGen(BaseRngT<T> base_rng);

    T operator()(T mean = 0., T stddev = 1.);

private:

    BaseRngT<T> base_rng_;

    bool is_cached_ = false;
    double cache_val_ = NAN;
};

template <std::floating_point T>
T NormalDistrib(T z) {
    return T(0.5) * (T(1.0) + std::erf(z / std::sqrt(T(2.0))));
}

template <std::floating_point T>
T NormalDistrib(T x, T mean, T stddev) {
    return NormalDistrib((x - mean) / stddev);
}

#include "generators/impl/NormalGen.tpp"