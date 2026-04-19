#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>


namespace minstd_rand {

void CorrectnessScalar(std::ostream& out, uint32_t seed, size_t iterations);
void CorrectnesVector(std::ostream& out, uint32_t seed, size_t iterations);

} // namespace minstd_rand