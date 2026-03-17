#pragma once

#include <concepts>
#include <cmath>

#include "common.hpp"


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

#include "impl/NormalGen.tpp"