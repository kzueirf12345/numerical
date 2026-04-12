#include <bit>
#include <climits>
#include <cstdlib>
#include <fstream>
#include <random>
#include <stdexcept>
#include <mpfr.h>

#include "CliParser/CliParser.hpp"
#include "Measurer/Measurer.hpp"

int main(int argc, char* argv[]) try {
    measurer::CliParser cli(argc, argv);
    
    if (cli.shouldExit()) {
        measurer::CliParser::printHelp(std::cout);
        return EXIT_SUCCESS;
    }

    const measurer::CliParser::Options& opts = cli.options();

    std::ostream* output_ptr = &std::cout;
    std::ofstream output_file;
    
    if (!opts.output_file.empty()) {
        output_file.open(opts.output_file);
        if (!output_file.is_open()) {
            throw std::invalid_argument("Can't open output file: " + opts.output_file);
        }
        output_ptr = &output_file;
    }


    double arg_libm = 0;
    std::mt19937_64 gen{opts.seed};
    std::uniform_real_distribution<double> dist(1., 1000.);

    auto log_libm       = [&arg_libm](){ return std::log(arg_libm); };
    auto log_setup_libm = [&arg_libm, &gen, &dist](){ 
        static_assert(sizeof(arg_libm) == sizeof(dist(gen)), "");
        arg_libm = std::bit_cast<double>(dist(gen));
    };

    mpfr_t arg_mpfr, log_val_mpfr;
    mpfr_init2(arg_mpfr, 53);
    mpfr_init2(log_val_mpfr, 53);

    auto log_mpfr = [&log_val_mpfr, &arg_mpfr](){ 
        mpfr_log(log_val_mpfr, arg_mpfr, MPFR_RNDN); 
    };
    auto log_setup_mpfr = [&arg_mpfr, &gen, &dist](){
        mpfr_set_d(arg_mpfr, std::bit_cast<double>(dist(gen)), MPFR_RNDN);
    };

    const measurer::Val log_res_libm = measurer::Runner::benchLatency(
        opts.buckets_cnt, opts.bucket_iterations_cnt, log_setup_libm, log_libm
    );
    const measurer::Val log_res_mpfr = measurer::Runner::benchLatency(
        opts.buckets_cnt, opts.bucket_iterations_cnt, log_setup_mpfr, log_mpfr
    );


    const double ratio = log_res_mpfr.mean / log_res_libm.mean; 

    const double rel_err_libm = log_res_libm.stddev / log_res_libm.mean;
    const double rel_err_mpfr = log_res_mpfr.stddev / log_res_mpfr.mean;
    const double ratio_stddev = ratio * sqrt(rel_err_libm * rel_err_libm  + rel_err_mpfr * rel_err_mpfr); 

    *output_ptr <<
        "\n===Benchmarking===\n"
        "buckets_cnt:       " << opts.buckets_cnt                                    << "\n"
        "iterations_cnt:    " << opts.bucket_iterations_cnt                          << "\n"
        "clks_mpfr:         " << log_res_mpfr.mean << " +/- " << log_res_mpfr.stddev << "\n"
        "clks_libm:         " << log_res_libm.mean << " +/- " << log_res_libm.stddev << "\n"
        "mpfr_div_libm:     " << ratio             << " +/- " << ratio_stddev        << "\n";

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