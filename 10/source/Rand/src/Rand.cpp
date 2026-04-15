#include "Rand/Rand.hpp"

namespace minstd_rand {

MinstdRand::MinstdRand() noexcept {}

MinstdRand::MinstdRand(uint32_t seed)
    :   prev_val_(seed)
{
    assert(seed % MODULUS_ != 0 && "Invalid state");
}

uint32_t MinstdRand::operator()() noexcept {
    const uint64_t product = (uint64_t)MULTIPLIER_ * prev_val_;
    const uint32_t lo = (uint32_t)product & MODULUS_;
    const uint32_t hi = (uint32_t)(product >> MODULUS_POW_);
    const uint32_t sum = lo + hi;

    const uint32_t sum_lo = sum & MODULUS_;
    const uint32_t sum_hi = sum >> MODULUS_POW_;

    prev_val_ = sum_hi + sum_lo;

    return prev_val_;
}

} // namespace minstd_rand