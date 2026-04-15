#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <random>
#include <stdexcept>

#include "CliParser/CliParser.hpp"
#include "Rand/Rand.hpp"
#include "Measurer/Measurer.hpp"

/*!SECTION

===Latency Benchmark===
std::minstd_rand         Mean: 114.00000  clks | StdDev: 0.00000 clks
My MinstdRand            Mean: 114.00000  clks | StdDev: 0.00000 clks
My MinstdRandVec         Mean: 7.12500    clks | StdDev: 0.00000 clks

===Throughput Benchmark===
std::minstd_rand         Mean: 17.89548   clks | StdDev: 0.00341 clks
My MinstdRand            Mean: 8.94688    clks | StdDev: 0.00137 clks
My MinstdRandVec         Mean: 2.25551    clks | StdDev: 0.00057 clks

*/

void check_scalar(std::ostream& out, uint32_t seed, size_t iterations);
void check_vector(std::ostream& out, uint32_t seed, size_t iterations);

void bench_latency(std::ostream& out, uint32_t seed, size_t buckets, size_t iterations);

void bench_throughput(
    std::ostream& out, uint32_t seed, size_t buckets, size_t batches, size_t iterations
);

int main(int argc, char* argv[]) try {
    CliParser cli(argc, argv);
    
    if (cli.shouldExit()) {
        CliParser::printHelp(std::cout);
        return EXIT_SUCCESS;
    }

    const CliParser::Options& opts = cli.options();

    std::ostream* output_ptr = &std::cout;
    std::ofstream output_file;
    
    if (!opts.output_file.empty()) {
        output_file.open(opts.output_file);
        if (!output_file.is_open()) {
            throw std::invalid_argument("Can't open output file: " + opts.output_file);
        }
        output_ptr = &output_file;
    }

    switch (opts.mode) {
        case CliParser::Mode::TESTING: {
            check_scalar(*output_ptr, opts.seed, opts.iterations_cnt);
            check_vector(*output_ptr, opts.seed, opts.iterations_cnt);
            break;
        }
        case CliParser::Mode::BENCH_LATENCY: {
            bench_latency(*output_ptr, opts.seed, opts.buckets_cnt, opts.iterations_cnt);
            break;
        }
        case CliParser::Mode::BENCH_THROUGHPUT: {
            bench_throughput(
                *output_ptr, opts.seed, opts.buckets_cnt, opts.batches_cnt, opts.iterations_cnt
            );
            break;
        }
        default: {
            throw std::invalid_argument("Unkown mode");
        }
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

void bench_throughput(
    std::ostream& out, uint32_t seed, size_t buckets, size_t batches, size_t iterations
) {
    std::minstd_rand std_gen(seed);
    auto std_rand_func = [&std_gen](size_t /*i*/) {
        return std_gen();
    };

    const measurer::Val res_std = measurer::Runner::benchThroughput(
        buckets, batches, iterations, measurer::Runner::VoidSetup, std_rand_func
    );

    minstd_rand::MinstdRand my_scalar_gen(seed);
    auto my_scalar_func = [&my_scalar_gen](size_t /*i*/) {
        return my_scalar_gen();
    };

    const measurer::Val res_scalar = measurer::Runner::benchThroughput(
        buckets, batches, iterations, measurer::Runner::VoidSetup, my_scalar_func
    );

    minstd_rand::MinstdRandVec my_vec_gen(seed);
    auto my_vec_func = [&my_vec_gen](size_t /*i*/) {
        return my_vec_gen();
    };

    const measurer::Val res_vec = measurer::Runner::benchThroughput(
        buckets, batches, iterations, measurer::Runner::VoidSetup, my_vec_func
    );

    auto print_row = [&out](const std::string& name, measurer::Val v, double factor = 1.0) {
        out << std::left << std::setw(25) << name 
            << std::fixed << std::setprecision(5)
            << "Mean: " << std::setw(10) << v.mean * factor << " clks | "
            << "StdDev: " << v.stddev * factor << " clks" << std::endl;
    };
    
    out << "\n===Throughput Benchmark===\n";
    print_row("std::minstd_rand", res_std);
    print_row("My MinstdRand", res_scalar);
    print_row("My MinstdRandVec", res_vec, 1.0 / 16.0);
}

void bench_latency(std::ostream& out, uint32_t seed, size_t buckets, size_t iterations) {

    std::minstd_rand std_gen(seed);
    auto std_rand_func = [&std_gen]() {
        return std_gen();
    };

    const measurer::Val res_std = measurer::Runner::benchLatency(
        buckets, iterations, measurer::Runner::VoidSetup, std_rand_func
    );

    minstd_rand::MinstdRand my_scalar_gen(seed);
    auto my_scalar_func = [&my_scalar_gen]() {
        my_scalar_gen();
    };

    const measurer::Val res_scalar = measurer::Runner::benchLatency(
        buckets, iterations, measurer::Runner::VoidSetup, my_scalar_func
    );

    minstd_rand::MinstdRandVec my_vec_gen(seed);
    auto my_vec_func = [&my_vec_gen]() {
        my_vec_gen();
    };

    const measurer::Val res_vec = measurer::Runner::benchLatency(
        buckets, iterations, measurer::Runner::VoidSetup, my_vec_func
    );

    auto print_row = [&out](const std::string& name, measurer::Val v, double factor = 1.0) {
        out << std::left << std::setw(25) << name 
            << std::fixed << std::setprecision(5)
            << "Mean: " << std::setw(10) << v.mean * factor << " clks | "
            << "StdDev: " << v.stddev * factor << " clks" << std::endl;
    };
    
    out << "\n===Latency Benchmark===\n";
    print_row("std::minstd_rand", res_std);
    print_row("My MinstdRand", res_scalar);
    print_row("My MinstdRandVec", res_vec, 1.0 / 16.0);
}


void check_scalar(std::ostream& out, uint32_t seed, size_t iterations) {
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

void check_vector(std::ostream& out, uint32_t seed, size_t iterations) {
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
