#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <random>
#include <omp.h>

#include "CliParser/CliParser.hpp"

#include "generators/ChiSquareGen.hpp"
#include "testing/Kolmagorov.hpp"

template <std::floating_point T>
void Chi2ExportSamples(
    std::ostream& out, 
    const std::vector<T>& samples, 
    uint64_t degree, 
    uint64_t seed
);

//==========================================MAIN====================================================

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

    const uint64_t master_seed = opts.seed.has_value() ? opts.seed.value() : std::random_device{}();


    constexpr size_t P_VALUES_CNT = 10000;
    std::vector<double> kolm_p_values(P_VALUES_CNT);

    std::vector<uint32_t> iteration_seeds(P_VALUES_CNT);
    std::seed_seq seed_seq{static_cast<uint32_t>(master_seed), static_cast<uint32_t>(master_seed >> 32)};  
    seed_seq.generate(iteration_seeds.begin(), iteration_seeds.end());

    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < P_VALUES_CNT; ++i) {
        const std::mt19937 gen{iteration_seeds[i]};
        const std::uniform_real_distribution<double> dist(0.0, 1.0);
        const BaseRngT<double> rng = std::bind(dist, gen);

        ChiSquareGen<double> chi2_gen(rng, opts.degree);

        std::vector<double> samples(opts.n);

        for (size_t j = 0; j < opts.n; ++j) {
            const double sample = chi2_gen();
            samples[j] = sample; 
        }

        const Kolmagorov<double>::AnalitFoo analit_chi2 = 
            std::bind(&ChiSquareGen<double>::analit, &chi2_gen, std::placeholders::_1);

        kolm_p_values[i] = Kolmagorov<double>::computePValue(samples, analit_chi2);
    }

    const double kolm_p_value = Kolmagorov<double>::computePValue(kolm_p_values);

    std::cout << "Kolmagorov p-value's p_value: " << kolm_p_value  << '\n';

    (void)output_ptr;
    // Chi2ExportSamples(*output_ptr, samples, opts.degree, seed);

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

//==========================================IMPLEMENTATIONS=========================================

template <std::floating_point T>
void Chi2ExportSamples(
    std::ostream& out, 
    const std::vector<T>& samples, 
    uint64_t degree, 
    uint64_t seed
) {
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