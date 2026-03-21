#pragma once

#include <concepts>
#include <functional>
#include <vector>
#include <cmath>

//https://portal.tpu.ru/SHARED/t/TOKTV/Page_121/Tab2/TabRasprKolmVilk.pdf

template <std::floating_point T>
class Kolmagorov {

public:
    
    using DistribFoo = std::function<T(T)>;

    static T DefaultDistrib(T x) { return x; }

    static T computeStatistic(std::vector<T> samples, const DistribFoo distrib = DefaultDistrib);

    static T computePValue(std::vector<T> samples, const DistribFoo distrib = DefaultDistrib);
private:


};

#include "testing/impl/Kolmagorov.tpp"

