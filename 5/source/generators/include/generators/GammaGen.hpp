#pragma once

#include <concepts>
#include <cstdint>

#include "ExpGen.hpp"


template <std::floating_point T>
class GammaGen {

public:

    GammaGen(BaseRngT<T> base_rng, uint64_t k, T theta);

    T operator()();

private:

    ExpGen<T> exp_gen_;

    uint64_t k_;
    T theta_;
};

#include "impl/GammaGen.tpp"