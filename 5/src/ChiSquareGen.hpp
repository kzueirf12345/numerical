#pragma once

#include <concepts>
#include <cstdint>

#include "GammaGen.hpp"
#include "NormalGen.hpp"


template <std::floating_point T>
class ChiSquareGen {

public:

    ChiSquareGen(BaseRngT<T> base_rng, uint64_t degree) 
        :   gamma_gen_(base_rng, degree / 2, 2)
        ,   normal_gen_(base_rng)
        ,   degree_(degree)
        ,   use_gamma(degree_ % 2 == 0)
    {}

    T operator()() {
        if (!use_gamma) {
            return genWithNormal();
        }

        return gamma_gen_();
    }

private:

    T genWithNormal() {
        T sum = 0;

        for (uint64_t i = 0; i < degree_; ++i) {
            const T num = normal_gen_();
            sum += num * num;
        }

        return sum;
    }

private:

    GammaGen<T> gamma_gen_;
    NormalGen<T> normal_gen_;

    uint64_t degree_;
    bool use_gamma;
};