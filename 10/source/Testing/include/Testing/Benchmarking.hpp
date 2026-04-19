#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>


namespace minstd_rand {

void BenchLatency(std::ostream& out, uint32_t seed, size_t buckets, size_t iterations);

void BenchThroughput(
    std::ostream& out, uint32_t seed, size_t buckets, size_t batches, size_t iterations
);

void BenchPi(
    std::ostream& out, uint32_t seed, size_t iterations
);

} // namespace minstd_rand