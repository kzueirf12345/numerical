#include <iostream>
#include <cstdlib>
#include <fstream>
#include <random>

#include "CliParser/CliParser.hpp"
#include "Rand/Rand.hpp"

int main(int argc, char* argv[]) {
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

    minstd_rand::MinstdRand my_gen(opts.seed);
    std::minstd_rand std_gen(opts.seed);

    for (size_t i = 0; i < 100; ++i) {
        *output_ptr << "my: " << my_gen() << "; std: " << std_gen() << std::endl;
    }

    return EXIT_SUCCESS;
}