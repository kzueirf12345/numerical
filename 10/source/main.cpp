#include <iostream>
#include <cstdlib>
#include <fstream>
#include <random>
#include <stdexcept>

#include "CliParser/CliParser.hpp"
#include "Rand/Rand.hpp"

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
            // TODO implement
            break;
        }
        case CliParser::Mode::BENCH_THROUGHPUT: {
            // TODO implement
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