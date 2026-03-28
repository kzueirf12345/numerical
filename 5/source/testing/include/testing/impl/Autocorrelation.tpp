#pragma once

#include <climits>
#include <cmath>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <bit>

#include "testing/Autocorrelation.hpp"

uint64_t Autocorrelation::computeStatistic(const std::vector<uint64_t>& samples, size_t lag) {

    const size_t n = samples.size();

    const size_t lag_div = lag / (sizeof(uint64_t) * CHAR_BIT);
    const size_t lag_rem = lag % (sizeof(uint64_t) * CHAR_BIT);
    const size_t lag_rem_inv = (sizeof(uint64_t) * CHAR_BIT) - lag_rem;

    uint64_t res = 0;

    for (size_t chunk_ind = 0; chunk_ind + lag_div < n; ++chunk_ind) {
        const size_t shifted_chunk_ind = chunk_ind + lag_div;

        uint64_t shifted_chunk = 0;

        shifted_chunk |= samples[shifted_chunk_ind] << lag_rem;

        if (lag_rem != 0) {
            if (shifted_chunk_ind + 1 < n) {
                shifted_chunk |= samples[shifted_chunk_ind + 1] >> lag_rem_inv;
            }
            else {
                shifted_chunk |= samples[chunk_ind] & (~(UINT64_C(0)) >> lag_rem_inv);
            }
        }
        
        res += std::popcount(samples[chunk_ind] ^ shifted_chunk);
    }

    // std::cerr << "lol\n";

    return res;
}