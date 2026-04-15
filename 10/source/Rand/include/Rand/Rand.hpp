#pragma once

#include <cassert>
#include <concepts>
#include <cstdint>

namespace minstd_rand {

class MinstdRand {

private:

    static inline constexpr const uint32_t MULTIPLIER_ = UINT32_C(48271);
    static inline constexpr const uint32_t MODULUS_POW_ = 31;
    static inline constexpr const uint32_t MODULUS_ = ((UINT32_C(1) << MODULUS_POW_) - 1);

public:

    MinstdRand() noexcept;

    MinstdRand(uint32_t seed);

    uint32_t operator()() noexcept;

    template <std::floating_point T>
    T get_0_1() noexcept {
        constexpr const T INV_MODULUS = static_cast<T>(1) / static_cast<T>(MODULUS_);
        return static_cast<T>(operator()()) * INV_MODULUS;
    }

private:

    uint32_t prev_val_ = 1;

};

} // namespace minstd_rand