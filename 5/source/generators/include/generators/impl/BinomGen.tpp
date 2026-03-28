#pragma once

#include <cstdint>
#include <concepts>
#include <stdexcept>
#include <cmath>
#include <cassert>

#include "generators/BinomGen.hpp"

template <std::floating_point T>
T BinomDistrib(uint64_t x, uint64_t n, T p) {
    if (p < 0 || p > 1) {
        throw std::invalid_argument("p in Binom must be in [0, 1]");
    }
    assert(x < n);
    const T q = 1 - p;
    const T q_n = std::pow(q, n); 
    T temp = q_n;
    T sum = 0;
    for (size_t k = 1; k <= x + 1; ++k) {
        sum += temp;
        temp *= static_cast<T>(n - (k - 1)) / k * p / q;
    }

    return sum;
}