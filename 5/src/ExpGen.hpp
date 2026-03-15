#pragma once

#include <concepts>
#include <cmath>
#include <stdexcept>

#include "common.hpp"


template <std::floating_point T>
class ExpGen {

public:

    ExpGen(BaseRngT<T> base_rng, T lambda) 
        :   base_rng_(base_rng)
        ,   lambda_(lambda)
    {
        if (lambda_ <= T(0)) {
            throw std::invalid_argument("Lambda must be positive");
        }
    }

    T operator()() {
        return -std::log(base_rng_()) / lambda_;
    }

private:

    BaseRngT<T> base_rng_;

    T lambda_;
};