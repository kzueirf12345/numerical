#pragma once

#include <cstdint>
#include <concepts>

template <std::floating_point T>
T BinomDistrib(uint64_t x, uint64_t n, T p);

#include "generators/impl/BinomGen.tpp"