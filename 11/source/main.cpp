#include <bit>
#include <climits>
#include <cstdlib>
#include <fstream>
#include <random>
#include <stdexcept>
#include <mpfr.h>

#include "CliParser/CliParser.hpp"
#include "Measurer/Measurer.hpp"

static void LatencyMode(std::ostream& out, const CliParser::Options& opts);
static void ThroughputMode(std::ostream& out, const CliParser::Options& opts);

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
        case CliParser::Mode::LATENCY: {
            LatencyMode(*output_ptr, opts);
            break;
        }

        case CliParser::Mode::THROUGHPUT: {
            ThroughputMode(*output_ptr, opts);
            break;
        }

        default: {
            std::cerr << "Unhandled mode!" << std::endl;
            return EXIT_FAILURE;
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

void LatencyMode(std::ostream& out, const CliParser::Options& opts) {
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
        opts.buckets_cnt, opts.iterations_cnt, log_setup_libm, log_libm
    );
    const measurer::Val log_res_mpfr = measurer::Runner::benchLatency(
        opts.buckets_cnt, opts.iterations_cnt, log_setup_mpfr, log_mpfr
    );

    const double ratio = log_res_mpfr.mean / log_res_libm.mean; 

    const double rel_err_libm = log_res_libm.stddev / log_res_libm.mean;
    const double rel_err_mpfr = log_res_mpfr.stddev / log_res_mpfr.mean;
    const double ratio_stddev = ratio * sqrt(rel_err_libm * rel_err_libm  + rel_err_mpfr * rel_err_mpfr); 

    out <<
        "\n===Benchmarking Latency===\n"
        "buckets_cnt:       " << opts.buckets_cnt                                    << "\n"
        "iterations_cnt:    " << opts.iterations_cnt                                 << "\n"
        "clks_mpfr:         " << log_res_mpfr.mean << " +/- " << log_res_mpfr.stddev << "\n"
        "clks_libm:         " << log_res_libm.mean << " +/- " << log_res_libm.stddev << "\n"
        "mpfr_div_libm:     " << ratio             << " +/- " << ratio_stddev        << "\n";
}

void ThroughputMode(std::ostream& out, const CliParser::Options& opts) {
    std::mt19937_64 gen{opts.seed};
    std::uniform_real_distribution<double> dist(1., 1000.);

    std::vector<double> batch_args_libm(opts.iterations_cnt);

    auto log_libm = [&batch_args_libm](const size_t i){ 
        return std::log(batch_args_libm[i]); 
    };
    auto log_setup_libm = [&batch_args_libm, &gen, &dist](){ 
        static_assert(sizeof(*batch_args_libm.data()) == sizeof(dist(gen)), "");
        for (double& arg : batch_args_libm) {
            arg = std::bit_cast<double>(dist(gen));
        }
    };

    const size_t batch_log_vals_mpfr_size = 10;

    std::vector<mpfr_t> batch_args_mpfr(opts.iterations_cnt);
    std::vector<mpfr_t> batch_log_vals_mpfr(batch_log_vals_mpfr_size);
    for (size_t ind = 0; ind < opts.iterations_cnt; ++ind) {
        mpfr_init2(batch_args_mpfr[ind], 53);
    }
    for (size_t ind = 0; ind < batch_log_vals_mpfr_size; ++ind) {
        mpfr_init2(batch_log_vals_mpfr[ind], 53);
    }

    auto log_mpfr = [&batch_args_mpfr, &batch_log_vals_mpfr](const size_t i){ 
        mpfr_log(batch_log_vals_mpfr[i % batch_log_vals_mpfr_size], batch_args_mpfr[i], MPFR_RNDN); 
    };
    auto log_setup_mpfr = [&batch_args_mpfr, &gen, &dist](){
        for (mpfr_t& arg : batch_args_mpfr) {
            mpfr_set_d(arg, std::bit_cast<double>(dist(gen)), MPFR_RNDN);
        }
    };

    const measurer::Val log_res_libm = measurer::Runner::benchThroughput(
        opts.buckets_cnt, opts.batches_cnt, opts.iterations_cnt, log_setup_libm, log_libm
    );
    const measurer::Val log_res_mpfr = measurer::Runner::benchThroughput(
        opts.buckets_cnt, opts.batches_cnt, opts.iterations_cnt, log_setup_mpfr, log_mpfr
    );

    const double ratio = log_res_mpfr.mean / log_res_libm.mean; 

    const double rel_err_libm = log_res_libm.stddev / log_res_libm.mean;
    const double rel_err_mpfr = log_res_mpfr.stddev / log_res_mpfr.mean;
    const double ratio_stddev = ratio * sqrt(rel_err_libm * rel_err_libm  + rel_err_mpfr * rel_err_mpfr); 

    out <<
        "\n===Benchmarking Throughput===\n"
        "buckets_cnt:       " << opts.buckets_cnt                                    << "\n"
        "batches_cnt:       " << opts.batches_cnt                                    << "\n"
        "iterations_cnt:    " << opts.iterations_cnt                                 << "\n"
        "clks_mpfr:         " << log_res_mpfr.mean << " +/- " << log_res_mpfr.stddev << "\n"
        "clks_libm:         " << log_res_libm.mean << " +/- " << log_res_libm.stddev << "\n"
        "mpfr_div_libm:     " << ratio             << " +/- " << ratio_stddev        << "\n";
}