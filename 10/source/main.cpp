#include <iostream>
#include <cstdlib>
#include <fstream>

#include "CliParser/CliParser.hpp"

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

    *output_ptr << "Hello, wolrd!\n";

    return EXIT_SUCCESS;
}