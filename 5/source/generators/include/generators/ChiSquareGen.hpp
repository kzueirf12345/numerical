#pragma once

#include <cstdint>

#include "GammaGen.hpp"
#include "NormalGen.hpp"


template <std::floating_point T>
class ChiSquareGen {

public:

    ChiSquareGen(BaseRngT<T> base_rng, uint64_t degree);

    T operator()();

private:

    T genWithNormal();

private:

    GammaGen<T> gamma_gen_;
    NormalGen<T> normal_gen_;

    uint64_t degree_;
    bool use_gamma;
};

#include "impl/ChiSquareGen.tpp"