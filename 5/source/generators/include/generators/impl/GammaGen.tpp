#pragma once

#include "generators/GammaGen.hpp"

template <std::floating_point T>
GammaGen<T>::GammaGen(BaseRngT<T> base_rng, uint64_t k, T theta) 
    :   exp_gen_(base_rng, 1 / theta)
    ,   k_(k)
    ,   theta_(theta)
{}

template <std::floating_point T>
T GammaGen<T>::operator()() {
    T sum = 0;

    for (size_t i = 0; i < k_; ++i) {
        sum += exp_gen_();
    }

    return sum;
}