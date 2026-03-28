#pragma once

#include <vector>
#include <cmath>
#include <cstdint>

class Autocorrelation {

public:

    static uint64_t computeStatistic(const std::vector<uint64_t>& samples, size_t lag);
    
};

#include "testing/impl/Autocorrelation.tpp"

