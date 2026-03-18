#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <random>
#include <omp.h>
#include <stdexcept>

#include "CliParser/CliParser.hpp"

#include "generators/ChiSquareGen.hpp"
#include "testing/Kolmagorov.hpp"

static void Chi2Export(const CliParser::Options& opts, std::ostream& out);
static void TestingChi2(const CliParser::Options& opts, std::ostream& out, uint64_t seed);
static void TestingRng(const CliParser::Options& opts, std::ostream& out, uint64_t seed);

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

    const uint64_t seed = opts.seed.has_value() ? opts.seed.value() : std::random_device{}();

    switch (opts.mode) {
        case CliParser::Mode::Chi2Export: {
            if (opts.verbose) {
                std::cout << "Start Chi2Export\n";
            }

            Chi2Export(opts, *output_ptr);

            if (opts.verbose) {
                std::cout << "Finish Chi2Export. Result output in '" << opts.output_file << "'\n";
            }
            break;
        }
        case CliParser::Mode::TestingChi2: {
            if (opts.verbose) {
                std::cout << "Start TestingChi2\n";
            }

            TestingChi2(opts, *output_ptr, seed);

            if (opts.verbose) {
                std::cout << "Finish TestingChi2. Result output in '" << opts.output_file << "'\n";
            }
            break;
        }
        case CliParser::Mode::TestingRng: {
            if (opts.verbose) {
                std::cout << "Start TestingRng\n";
            }

            TestingRng(opts, *output_ptr, seed);

            if (opts.verbose) {
                std::cout << "Finish TestingRng. Result output in '" << opts.output_file << "'\n";
            }
            break;
        }
        default:
            throw std::invalid_argument("Unknown mode");
            break;
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

//==========================================IMPLEMENTATIONS=========================================

//{{{-----------------------------------------HELPERS-----------------------------------------------

template <std::floating_point T>
void Chi2ExportSamples(
    std::ostream& out, 
    const std::vector<T>& samples, 
    uint64_t degree, 
    uint64_t seed
);

static void TestChi2(std::ostream& out, uint64_t seed, size_t n, uint64_t degree);
static void TestRng(std::ostream& out, uint64_t seed, size_t n);

//}}}-----------------------------------------HELPERS-----------------------------------------------

void Chi2Export(const CliParser::Options& opts, std::ostream& out) {
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

    Chi2ExportSamples(out, samples, opts.degree, seed);
}

void TestingChi2(const CliParser::Options& opts, std::ostream& out, uint64_t seed) {
    out << "{\n"
        "\"seed\": " << seed << ",\n"
        "\"name\": \"TestsChi2\",\n"
        "\"description\": \"Two-pass test Chi2 generator with base RNG mt19937. "
        "Сompute p-values by Kolmagorov statistic, and final p-value with this p-values\",\n"
        "\"tests\": [\n";

    std::vector<uint32_t> seeds(opts.tests_cnt);
    std::seed_seq seed_seq{static_cast<uint32_t>(seed), static_cast<uint32_t>(seed >> 32)};  
    seed_seq.generate(seeds.begin(), seeds.end());

    for (size_t test_num = 0; test_num < opts.tests_cnt; ++test_num) {
        if (opts.verbose) {
            std::cout << "Test №" << test_num << "...\n";
        }
        TestChi2(out, seeds[test_num], opts.n, opts.degree);
        if (test_num + 1 != opts.tests_cnt) {
            out << ",";
        }
        out << "\n";
    }
    out << "]\n}";
}

void TestingRng(const CliParser::Options& opts, std::ostream& out, uint64_t seed) {
    out << "{\n"
        "\"seed\": " << seed << ",\n"
        "\"name\": \"TestsRng\",\n"
        // "\"description\": \"Two-pass test Chi2 generator with base RNG mt19937. "
        // "Сompute p-values by Kolmagorov statistic, and final p-value with this p-values\",\n"
        "\"tests\": [\n";

    std::vector<uint32_t> seeds(opts.tests_cnt);
    std::seed_seq seed_seq{static_cast<uint32_t>(seed), static_cast<uint32_t>(seed >> 32)};  
    seed_seq.generate(seeds.begin(), seeds.end());

    for (size_t test_num = 0; test_num < opts.tests_cnt; ++test_num) {
        if (opts.verbose) {
            std::cout << "Test №" << test_num << "...\n";
        }
        TestRng(out, seeds[test_num], opts.n);
        if (test_num + 1 != opts.tests_cnt) {
            out << ",";
        }
        out << "\n";
    }
    out << "]\n}";
}



//-----------------------------------------HELPERS--------------------------------------------------

void TestRng(std::ostream& out, uint64_t seed, size_t n) {
    (void)seed;
    (void)n;
    out << "TODO ;)\n";
    //TODO implement
}

void TestChi2(std::ostream& out, uint64_t seed, size_t n, uint64_t degree) {
    const uint64_t master_seed = seed;

    std::vector<double> kolm_p_values(n);

    std::vector<uint32_t> seeds(n);
    std::seed_seq seed_seq{static_cast<uint32_t>(master_seed), static_cast<uint32_t>(master_seed >> 32)};  
    seed_seq.generate(seeds.begin(), seeds.end());

    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < n; ++i) {
        const std::mt19937 gen{seeds[i]};
        const std::uniform_real_distribution<double> dist(0.0, 1.0);
        const BaseRngT<double> rng = std::bind(dist, gen);

        ChiSquareGen<double> chi2_gen(rng, degree);

        std::vector<double> samples(n);

        for (size_t j = 0; j < n; ++j) {
            const double sample = chi2_gen();
            samples[j] = sample; 
        }

        const Kolmagorov<double>::AnalitFoo analit_chi2 = 
            std::bind(&ChiSquareGen<double>::analit, &chi2_gen, std::placeholders::_1);

        kolm_p_values[i] = Kolmagorov<double>::computePValue(samples, analit_chi2);
    }

    const double kolm_p_value = Kolmagorov<double>::computePValue(kolm_p_values);

    out << "{\n"
        "\t\"name\": \"TestChi2\",\n"
        "\t\"samples_cnt\": " << n << ",\n"
        "\t\"seed\": " << master_seed << ",\n"
        "\t\"degree\": " << degree << ",\n"
        "\t\"final_p_value\": " << kolm_p_value  << "\n"
    "}";
}

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