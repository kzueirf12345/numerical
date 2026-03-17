#pragma once

#include "../ChiSquareGen.hpp"

template <std::floating_point T>
ChiSquareGen<T>::ChiSquareGen(BaseRngT<T> base_rng, uint64_t degree) 
    :   gamma_gen_(base_rng, degree / 2, 2)
    ,   normal_gen_(base_rng)
    ,   degree_(degree)
    ,   use_gamma(degree_ % 2 == 0)
{}

template <std::floating_point T>
T ChiSquareGen<T>::operator()() {
    if (!use_gamma) {
        return genWithNormal();
    }

    return gamma_gen_();
}

template <std::floating_point T>
T ChiSquareGen<T>::genWithNormal() {
    T sum = 0;

    for (uint64_t i = 0; i < degree_; ++i) {
        const T num = normal_gen_();
        sum += num * num;
    }

    return sum;
}