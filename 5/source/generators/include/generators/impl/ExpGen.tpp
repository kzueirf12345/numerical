#pragma once

#include <stdexcept>

#include "../ExpGen.hpp"

template <std::floating_point T>
ExpGen<T>::ExpGen(BaseRngT<T> base_rng, T lambda) 
    :   base_rng_(base_rng)
    ,   lambda_(lambda)
{
    if (lambda_ <= T(0)) {
        throw std::invalid_argument("Lambda must be positive");
    }
}

template <std::floating_point T>
T ExpGen<T>::operator()() {
    return -std::log(base_rng_()) / lambda_;
}