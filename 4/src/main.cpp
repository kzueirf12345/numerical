#include <array>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <random>
#include <concepts>
#include <iostream>
#include <stdexcept>

template <std::floating_point T>
struct Parametrs {
    T mean;
    T stddev_or_disp;
};

template <std::floating_point T>
static Parametrs<T> DispFast(std::mt19937& gen, T real_mean, T real_stddev, size_t selection_cnt = 1000) {
    std::normal_distribution<T> distrib(real_mean, real_stddev);

    T RS = 0;
    T mean = 0; 
    for (size_t x_ind = 0; x_ind < selection_cnt; ++x_ind) {
        T x = distrib(gen);
        RS += x * x;
        mean += x;
    }

    RS = RS / static_cast<T>(selection_cnt);
    mean = mean / static_cast<T>(selection_cnt);

    T disp = RS - mean * mean;
    return {mean, disp};
}

template <std::floating_point T>
static Parametrs<T> Disp2Pass(std::mt19937& gen, T real_mean, T real_stddev, size_t selection_cnt = 1000) {
    std::normal_distribution<T> distrib(real_mean, real_stddev);

    std::vector<T> selection(selection_cnt);

    T mean = 0; 
    for (size_t x_ind = 0; x_ind < selection_cnt; ++x_ind) {
        T x = distrib(gen);
        mean += x;
        selection[x_ind] = x;
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


int main(int argc, char* argv[]) try {
    std::ostream* output_ptr = &std::cout;
    std::ofstream output_file{};
    
    if (argc == 2) {
        output_file.open(argv[1]);
        if (!output_file.is_open()) {
            throw std::invalid_argument("Can't open output file");
        }
        output_ptr = &output_file;
    } 
    else if (argc > 2) {
        throw std::invalid_argument("Too many command line args");
    }

    std::ostream& output = *output_ptr;

    std::random_device rd;
    std::mt19937 gen(rd());

    constexpr size_t test_iter_cnt = 100;

    output << "===Float32===\n";

    constexpr size_t parametrs_cnt = 3;

    constexpr std::array<Parametrs<float>, parametrs_cnt> parametrs_arr = {
        Parametrs<float>{1.f, 1.f}, 
        Parametrs<float>{10.f, 0.1f}, 
        Parametrs<float>{100.f, 0.01f}
    };

    output << "---Fast Method---\n";

    for (size_t parametrs_num = 0; parametrs_num < parametrs_cnt; ++parametrs_num) {
        const Parametrs<float> real_params = parametrs_arr[parametrs_num];
        const float real_mean = real_params.mean;
        const float real_stddev = real_params.stddev_or_disp;

        output << "real_mean = " << real_mean << "; real_disp = " << real_stddev * real_stddev << ";\n";

        for (size_t test_num = 0; test_num < test_iter_cnt; ++test_num) {
            const Parametrs<float> params = DispFast(gen, real_mean, real_stddev);
            const float mean = params.mean;
            const float disp = params.stddev_or_disp;

            output << std::fixed << std::left 
                << "Test "      << std::setw(4)     << test_num
                << "mean = "    << std::setw(15)    << mean
                << "disp = "    << std::setw(30)    << disp   
            << "\n";
        }

        output << '\n';
    }

    output << "---2 Pass Method---\n";

    for (size_t parametrs_num = 0; parametrs_num < parametrs_cnt; ++parametrs_num) {
        const Parametrs<float> real_params = parametrs_arr[parametrs_num];
        const float real_mean = real_params.mean;
        const float real_stddev = real_params.stddev_or_disp;

        output << "real_mean = " << real_mean << "; real_disp = " << real_stddev * real_stddev << ";\n";

        for (size_t test_num = 0; test_num < test_iter_cnt; ++test_num) {
            const Parametrs<float> params = Disp2Pass(gen, real_mean, real_stddev);
            const float mean = params.mean;
            const float disp = params.stddev_or_disp;

            output << std::fixed << std::left 
                << "Test "      << std::setw(4)     << test_num
                << "mean = "    << std::setw(15)    << mean
                << "disp = "    << std::setw(30)    << disp   
            << "\n";
        }

        output << '\n';
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