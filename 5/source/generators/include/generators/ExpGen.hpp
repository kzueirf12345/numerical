#pragma once

#include <concepts>
#include <cmath>

#include "generators/common.hpp"


template <std::floating_point T>
class ExpGen {

public:

    ExpGen(BaseRngT<T> base_rng, T lambda);

    T operator()();

private:

    BaseRngT<T> base_rng_;

    T lambda_;
};

#include "generators/impl/ExpGen.tpp"