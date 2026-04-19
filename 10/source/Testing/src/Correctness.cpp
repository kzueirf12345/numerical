#include "Testing/Correctness.hpp"

#include <random>

#include "Rand/Rand.hpp"

namespace minstd_rand {

void CorrectnessScalar(std::ostream& out, uint32_t seed, size_t iterations) {
    std::minstd_rand std_gen(seed);
    minstd_rand::MinstdRand my_gen(seed);

    for (size_t iteration = 0; iteration < iterations; ++iteration) {
        uint32_t expected = std_gen();
        uint32_t actual = my_gen();
        if (expected != actual) {
            out << "Scalar error at step " << iteration 
                << ": expected " << expected << ", got " << actual << std::endl;
            return;
        }
    }

    out << "Scalar test: OK" << std::endl;
}

void CorrectnesVector(std::ostream& out, uint32_t seed, size_t iterations) {
    std::minstd_rand std_gen(seed);
    minstd_rand::MinstdRandVec my_gen(seed);

    alignas(64) uint32_t results[16];

    for (size_t iteration = 0; iteration < iterations; ++iteration) {
        __m512i vec_res = my_gen();
        _mm512_store_si512(results, vec_res);

        for (size_t j = 0; j < 16; ++j) {
            uint32_t expected = std_gen();
            if (results[j] != expected) {
                out << "Vector error at iteration " << iteration << ", lane " << j 
                    << ": expected " << expected << ", got " << results[j] << std::endl;
                return;
            }
        }
    }

    out << "Vector test: OK" << std::endl;
}


} // namespace minstd_rand