#pragma once

#include <cstdint>

#include "generators/GammaGen.hpp"
#include "generators/NormalGen.hpp"


template <std::floating_point T>
class ChiSquareGen {

public:

    ChiSquareGen(BaseRngT<T> base_rng, uint64_t degree);

    T operator()();

    T analit(T x) const;

private:

    T genWithNormal();

private:

    GammaGen<T> gamma_gen_;
    NormalGen<T> normal_gen_;

    uint64_t degree_;
    bool is_even_degree_;
};

#include "generators/impl/ChiSquareGen.tpp"