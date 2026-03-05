#include <array>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <random>
#include <concepts>
#include <iostream>

template <std::floating_point T>
struct Parametrs {
    T mean;
    T stddev;
};

template <std::floating_point T>
static Parametrs<T> StddevFast(std::mt19937& gen, T real_mean, T real_stddev, size_t iter_cnt = 1000) {
    std::normal_distribution<T> distrib(real_mean, real_stddev);

    T RS = 0;
    T mean = 0; 
    for (size_t i = 0; i < iter_cnt; ++i) {
        T x = distrib(gen);
        RS += x * x;
        mean += x;
    }

    RS = RS / static_cast<T>(iter_cnt);
    mean = mean / static_cast<T>(iter_cnt);

    T stddev = RS - mean * mean;
    return {mean, stddev};
}


int main() try {
    std::random_device rd;
    std::mt19937 gen(rd());

    constexpr size_t test_iter_cnt = 100;

    std::cerr << "===Float32===\n";

    constexpr size_t parametrs_cnt = 3;

    constexpr std::array<Parametrs<float>, parametrs_cnt> parametrs_arr = {
        Parametrs<float>{1.f, 1.f}, 
        Parametrs<float>{10.f, 0.1f}, 
        Parametrs<float>{100.f, 0.01f}
    };

    std::cout << "---Fast Method---\n";

    for (size_t parametrs_num = 0; parametrs_num < parametrs_cnt; ++parametrs_num) {
        const Parametrs<float> real_params = parametrs_arr[parametrs_num];
        const float real_mean = real_params.mean;
        const float real_stddev = real_params.stddev;

        std::cout << "real_mean = " << real_mean << ";\t\treal_stddev = " << real_stddev << ";\n";

        for (size_t test_num = 0; test_num < test_iter_cnt; ++test_num) {
            const Parametrs<float> params = StddevFast(gen, real_mean, real_stddev);
            const float mean = params.mean;
            const float stddev = params.stddev;

            std::cout << std::fixed << std::left 
                << "Test "      << std::setw(4)     << test_num
                << "mean = "    << std::setw(15)    << mean
                << "stddev = "  << std::setw(30)    << stddev   
            << "\n";
        }

        std::cout << '\n';
    }


    return EXIT_SUCCESS;
}
catch(const std::exception& e) {
    std::cerr << "!!!EXCEPTION!!!\n" << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch(...) {
    std::cerr << "!!!EXCEPTION!!!\n" << "Something went wrong!" << std::endl;
    return EXIT_FAILURE;
}