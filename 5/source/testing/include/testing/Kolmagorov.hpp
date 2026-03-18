#pragma once

#include <concepts>
#include <functional>
#include <vector>
#include <cmath>

template <std::floating_point T>
class Kolmagorov {

public:

    //https://portal.tpu.ru/SHARED/t/TOKTV/Page_121/Tab2/TabRasprKolmVilk.pdf
    constexpr static inline T DEFAULT_SIGNIFICANCE_LEVEL = 0.05;
    constexpr static inline T DEFAULT_CRITICAL_VAL = 1.36;
    
    using AnalitFoo = std::function<T(T)>;

    static T DefaultAnalit(T x) { return x; }

    static T computeStatistic(std::vector<T> samples, const AnalitFoo analit = DefaultAnalit);

    static T computePValue(std::vector<T> samples, const AnalitFoo analit = DefaultAnalit);
private:


};

#include "testing/impl/Kolmagorov.tpp"

