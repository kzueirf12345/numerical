#pragma once

#include <concepts>
#include <cstdint>

#include "ExpGen.hpp"


template <std::floating_point T>
class GammaGen {

public:

    GammaGen(BaseRngT<T> base_rng, uint64_t k, T theta) 
        :   exp_gen_(base_rng, 1 / theta)
        ,   k_(k)
        ,   theta_(theta)
    {}

    T operator()() {
        T sum = 0;

        for (size_t i = 0; i < k_; ++i) {
            sum += exp_gen_();
        }

        return sum;
    }

private:

    ExpGen<T> exp_gen_;

    uint64_t k_;
    T theta_;
};