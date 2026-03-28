#pragma once

#include <cmath>
#include <concepts>

#include "testing/Kolmagorov.hpp"

template <std::floating_point T>
T Kolmagorov<T>::computeStatistic(std::vector<T> samples, const DistribFoo distrib) {
    std::sort(samples.begin(), samples.end());

    const size_t n = samples.size();

    T max_dev = 0;

    for (size_t ind = 0; ind < n; ++ind) {
        const T u = distrib(samples[ind]);
        const T dev_plus  = static_cast<T>(ind + 1) / n - u;
        const T dev_minus = u - static_cast<T>(ind) / n;
        max_dev = std::max(max_dev, std::max(dev_minus, dev_plus));
    }

    return std::sqrt(n) * max_dev;
}

template <std::floating_point T>
T Kolmagorov<T>::computePValue(std::vector<T> samples, const DistribFoo distrib) {
    T p_val = 0;
    T sign = 1;
    const T stat = computeStatistic(std::move(samples), distrib);
    const T stat_2 = stat * stat;

    constexpr T PRECISION = 1e-10;

    T term = PRECISION + 1;

    for (size_t iter = 1; term > PRECISION; ++iter) {
        term = std::exp(static_cast<T>(-2) * iter * iter * stat_2);
        p_val += sign * term;
        sign = -sign;
    }

    p_val *= 2;

    return p_val;
}