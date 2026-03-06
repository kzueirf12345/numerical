#pragma once 

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <mutex>
#include <random>
#include <concepts>
#include <iostream>
#include <type_traits>
#include <string>

template <std::floating_point T>
struct Parametrs {
    T mean;
    T stddev_or_disp;
};

class Tester {

public:

    using seed_t = unsigned int;

public:

    inline Tester();
    inline Tester(seed_t seed);

    template <std::floating_point T>
    inline void Test(
        std::ostream& output, 
        T real_mean, 
        T real_stddev, 
        size_t iters_cnt, 
        size_t selection_cnt = DEFAULT_SELECTION_CNT
    );

private:

    std::random_device rd_;
    seed_t seed_;
    std::mt19937 gen_;

    std::once_flag once_flag_seed_print_;

private:

    template <std::floating_point T>
    [[nodiscard]] static inline Parametrs<T> DispFast(const std::vector<T>& selection);

    template <std::floating_point T>
    [[nodiscard]] static inline Parametrs<T> Disp2Pass(const std::vector<T>& selection);

    template <std::floating_point T>
    [[nodiscard]] static inline Parametrs<T> Disp1Pass(const std::vector<T>& selection);

private:

    static inline constexpr size_t DEFAULT_SELECTION_CNT = 1000;

    static inline const std::string FLOAT32_NAME = "===FLOAT32===";
    static inline const std::string FLOAT64_NAME = "===FLOAT64===";

    static inline const std::string FAST_METHOD_NAME = "---Fast Method---";
    static inline const std::string TWO_PASS_METHOD_NAME = "---2 Pass Method---";
    static inline const std::string ONE_PASS_METHOD_NAME = "---1 Pass Method---";

    static inline const std::string REAL_MEAN_STR = "real_mean = ";
    static inline const std::string REAL_DISP_STR = "real_disp = ";

    static inline const std::string MEAN_STR = "mean = ";
    static inline const std::string DISP_STR = "disp = ";
    static inline const std::string TEST_STR = "Test ";

    static inline constexpr size_t TEST_COL_W = 5;
    static inline constexpr size_t MEAN_COL_W = 15;
    static inline constexpr size_t DISP_COL_W = 15;

    static inline const std::string TEST_SPACE_SKIP_STR = std::string(TEST_STR.size() + TEST_COL_W, ' ');
};

//======================================IMPLEMENTATION==============================================

Tester::Tester() 
    :   rd_()
    ,   seed_(rd_())
    ,   gen_(seed_)
{}

Tester::Tester(seed_t seed) 
    :   rd_()
    ,   seed_(seed)
    ,   gen_(seed_)
{}

template <std::floating_point T>
void Tester::Test(std::ostream& output, T real_mean, T real_stddev, size_t iters_cnt, size_t selection_cnt) {
    std::call_once(once_flag_seed_print_, [&output, this](){
        output << "seed: " << seed_ << '\n';
    });

    if constexpr (std::is_same_v<T, float>) {
        output << FLOAT32_NAME << '\n';
    } 
    else {
        output << FLOAT64_NAME << '\n';
    }

    output << REAL_MEAN_STR << real_mean << '\n' 
           << REAL_DISP_STR << real_stddev * real_stddev << '\n';

    const size_t colw = MEAN_COL_W + DISP_COL_W + MEAN_STR.size() + DISP_STR.size();

    output << std::left << TEST_SPACE_SKIP_STR
        << std::setw(static_cast<int>(colw)) << std::string(static_cast<size_t>((colw  -     FAST_METHOD_NAME.size()) / 2), ' ') + FAST_METHOD_NAME
        << std::setw(static_cast<int>(colw)) << std::string(static_cast<size_t>((colw  - TWO_PASS_METHOD_NAME.size()) / 2), ' ') + TWO_PASS_METHOD_NAME
        << std::setw(static_cast<int>(colw)) << std::string(static_cast<size_t>((colw  - ONE_PASS_METHOD_NAME.size()) / 2), ' ') + ONE_PASS_METHOD_NAME
    << '\n';

    std::normal_distribution<T> distrib(real_mean, real_stddev);

    for (size_t test_num = 0; test_num < iters_cnt; ++test_num) {
        std::vector<T> selection(selection_cnt);

        for (size_t x_ind = 0; x_ind < selection_cnt; ++x_ind)  {
            selection[x_ind] = distrib(gen_);
        }

        const Parametrs<T> params_fast  = DispFast(selection);
        const Parametrs<T> params_2pass = Disp2Pass(selection);
        const Parametrs<T> params_1pass = Disp1Pass(selection);

        output << std::fixed << std::left 
            << "Test "      << std::setw(TEST_COL_W)   << test_num                     << "|"
            << "mean = "    << std::setw(MEAN_COL_W)   << params_fast.mean             << "|"
            << "disp = "    << std::setw(DISP_COL_W)   << params_fast.stddev_or_disp   << "|" 
            << "mean = "    << std::setw(MEAN_COL_W)   << params_2pass.mean            << "|"
            << "disp = "    << std::setw(DISP_COL_W)   << params_2pass.stddev_or_disp  << "|"
            << "mean = "    << std::setw(MEAN_COL_W)   << params_1pass.mean            << "|"
            << "disp = "    << std::setw(DISP_COL_W)   << params_1pass.stddev_or_disp  << "|"
        << "\n";
    }

    output << '\n';
}

template <std::floating_point T>
Parametrs<T> Tester::DispFast(const std::vector<T>& selection) {
    const size_t selection_cnt = selection.size();
    
    T RS = 0;
    T mean = 0; 
    for (size_t x_ind = 0; x_ind < selection_cnt; ++x_ind) {
        T x = selection[x_ind];
        RS += x * x;
        mean += x;
    }

    RS = RS / static_cast<T>(selection_cnt);
    mean = mean / static_cast<T>(selection_cnt);

    T disp = RS - mean * mean;
    return {mean, disp};
}

template <std::floating_point T>
Parametrs<T> Tester::Disp2Pass(const std::vector<T>& selection) {
    const size_t selection_cnt = selection.size();

    T mean = 0; 
    for (size_t x_ind = 0; x_ind < selection_cnt; ++x_ind) {
        const T x = selection[x_ind];
        mean += x;
    }

    mean = mean / static_cast<T>(selection_cnt);

    T disp = 0;

    for (size_t x_ind = 0; x_ind < selection_cnt; ++x_ind) {
        const T x = selection[x_ind];
        const T dev = x - mean;
        disp += dev * dev;
    }

    disp = disp / static_cast<T>(selection_cnt);

    return {mean, disp};
}

template <std::floating_point T>
Parametrs<T> Tester::Disp1Pass(const std::vector<T>& selection) {
    const size_t selection_cnt = selection.size();

    T x = selection[0];
    T mean = x; 
    T disp = 0;
    for (size_t x_ind = 1; x_ind < selection_cnt; ++x_ind) {
        x = selection[x_ind];
        const T old_mean = mean;
        mean += (x - mean) / static_cast<T>(x_ind);
        disp += ((x - old_mean) * (x - mean) - disp) / static_cast<T>(x_ind);
    }
    
    return {mean, disp};
}