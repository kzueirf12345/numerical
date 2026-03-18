#pragma once

#include <cmath>
#include <concepts>
#include <numbers>

#include "generators/ChiSquareGen.hpp"

template <std::floating_point T>
ChiSquareGen<T>::ChiSquareGen(BaseRngT<T> base_rng, uint64_t degree) 
    :   gamma_gen_(base_rng, degree / 2, 2)
    ,   normal_gen_(base_rng)
    ,   degree_(degree)
    ,   is_even_degree_(degree_ % 2 == 0)
{}

template <std::floating_point T>
T ChiSquareGen<T>::operator()() {
    if (!is_even_degree_) {
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

template <std::floating_point T>
T ChiSquareGen<T>::analit(T x) const {
    const T half_x = x / 2;
    const T exp_neg_half_x = std::exp(-half_x);

    if (is_even_degree_) {
        T sum = 0;

        T temp = 1;

        for (size_t i = 1; i <= degree_ / 2; ++i) {
            sum += temp;
            temp *= half_x / i;
        }

        return 1 - exp_neg_half_x * sum;
    }

    const T erf_term = std::erf(std::sqrt(half_x));

    if (degree_ == 1) {
        return erf_term;
    }

    T sum = 0;
    T temp = std::sqrt(x);

    for (size_t i = 1; 2 * i <= degree_ - 1; ++i) {
        sum += temp;
        temp *= x / (2 * i + 1);
    }

    constexpr T REV_HALF_PI = std::sqrt(2 / std::numbers::pi_v<T>);

    return erf_term - exp_neg_half_x * REV_HALF_PI * sum;

}