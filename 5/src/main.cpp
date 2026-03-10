#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <random>

#include "CliParser.hpp"

#include "ChiSquareGen.hpp"


template <std::floating_point T>
void Chi2ExportSamples(std::ostream& out, const std::vector<T>& samples, uint64_t degree, uint64_t seed) {
    out << "{\n"
           "\t\"seed\": " << seed << ",\n"
           "\t\"degree\": " << degree << ",\n"
           "\t\"value\": [\n";

    const size_t samples_cnt = samples.size();

    for (size_t i = 0; i < samples_cnt - 1; ++i) {
        out << "\t\t" << samples[i] << ",\n";
    }

    out << "\t\t" << samples[samples_cnt - 1] << '\n';

    out << "\t]\n"
           "}";
}

int main(int argc, char* argv[]) try {
    CliParser cli(argc, argv);
    
    if (cli.shouldExit()) {
        CliParser::PrintHelp(std::cout);
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

    const uint64_t seed = opts.seed.has_value() ? opts.seed.value() : std::random_device{}();
    const std::mt19937 gen{seed};
    const std::uniform_real_distribution<double> dist(0.0, 1.0);
    const BaseRngT<double> rng = std::bind(dist, gen);

    ChiSquareGen<double> chi2_gen(rng, opts.degree);

    std::vector<double> samples(opts.n);

    for (size_t i = 0; i < opts.n; ++i) {
        const double sample = chi2_gen();
        samples[i] = sample; 
    }

    Chi2ExportSamples(*output_ptr, samples, opts.degree, seed);

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