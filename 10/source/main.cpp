#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stdexcept>

#include "CliParser/CliParser.hpp"
#include "Testing/Correctness.hpp"
#include "Testing/Benchmarking.hpp"

/*!SECTION

===Latency Benchmark===
std::minstd_rand         Mean: 114.00000  clks | StdDev: 0.00000 clks
My MinstdRand            Mean: 114.00000  clks | StdDev: 0.00000 clks
My MinstdRandVec         Mean: 7.12500    clks | StdDev: 0.00000 clks

===Throughput Benchmark===
std::minstd_rand         Mean: 17.89548   clks | StdDev: 0.00341 clks
My MinstdRand            Mean: 8.94688    clks | StdDev: 0.00137 clks
My MinstdRandVec         Mean: 2.25551    clks | StdDev: 0.00057 clks

===Pi Benchmark===
                 Correct Pi:           3.1415265359
std::minstd_rand         Pi:           3.1427378800 | Time:      3837439918 clks
My MinstdRand            Pi:           3.1427378800 | Time:      2103907012 clks
My MinstdRandVec         Pi:           3.1427378800 | Time:       485354810 clks

// with barriers
===Pi Benchmark===
                 Correct Pi:           3.1415265359
std::minstd_rand         Pi:           3.1415043120 | Time:      2659005388 clks
My MinstdRand            Pi:           3.1415043120 | Time:      1625531700 clks
My MinstdRandVec         Pi:           3.1415043120 | Time:       341817068 clks


*/

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
            minstd_rand::CorrectnessScalar(*output_ptr, opts.seed, opts.iterations_cnt);
            minstd_rand::CorrectnesVector(*output_ptr, opts.seed, opts.iterations_cnt);
            break;
        }
        case CliParser::Mode::BENCH_LATENCY: {
            minstd_rand::BenchLatency(*output_ptr, opts.seed, opts.buckets_cnt, opts.iterations_cnt);
            break;
        }
        case CliParser::Mode::BENCH_THROUGHPUT: {
            minstd_rand::BenchThroughput(
                *output_ptr, opts.seed, opts.buckets_cnt, opts.batches_cnt, opts.iterations_cnt
            );
            break;
        }
        case CliParser::Mode::BENCH_PI: {
            minstd_rand::BenchPi(
                *output_ptr, opts.seed, opts.iterations_cnt
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