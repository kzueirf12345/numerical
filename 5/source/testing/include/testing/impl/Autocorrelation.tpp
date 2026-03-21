#pragma once

#include <climits>
#include <cmath>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <bit>
#include <iostream>

#include "testing/Autocorrelation.hpp"


// uint64_t Autocorrelation::computeStatistic(uint64_t bits, size_t lag) {
//     const uint64_t shifted = bits >> lag;
//     const uint64_t xored = bits ^ shifted;

//     const uint64_t mask = ~UINT64_C(0) >> lag;
//     const uint64_t masked = xored & mask;

//     return std::popcount(masked);
// }

// uint64_t Autocorrelation::computeStatistic(const std::vector<uint8_t>& samples, size_t lag) {
//     uint64_t res = 0;

//     const size_t lag_rem = lag % CHAR_BIT;
//     const bool lag_has_rem = (lag_rem != 0);

//     const size_t n_bytes = samples.size();
//     const size_t n_bits = n_bytes * CHAR_BIT;

//     const size_t xored_bits = n_bits - lag;
//     const size_t xored_bytes = xored_bits / CHAR_BIT;
//     const size_t xored_rem = CHAR_BIT - lag_rem;

//     for (size_t xored_byte_ind = 0; xored_byte_ind <= xored_bytes; ++xored_byte_ind) {
//         const size_t shifted_byte_ind = xored_byte_ind + lag / CHAR_BIT;

//         const uint8_t sample = samples[xored_byte_ind];

//         const uint8_t shifted1 = samples[shifted_byte_ind] << lag_rem;

//         uint8_t shifted2 = 0;
//         if (lag_has_rem) {
//             assert(shifted_byte_ind + 1 < n_bytes);
//             shifted2 = samples[shifted_byte_ind + 1] >> xored_rem;
//         }

//         const uint8_t shifted = shifted1 | shifted2;

//         uint8_t xored = sample ^ shifted;

//         if (xored_byte_ind == xored_bytes) {
//             xored &= (0xFF << lag_rem);
//         }

//         res += std::popcount(xored);
//     }

//     return res;
// }

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