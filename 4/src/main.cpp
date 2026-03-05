#include <cmath>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "CliParser.hpp"
#include "Tester.hpp"

//======================================MAIN========================================================

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

    constexpr size_t test_iter_cnt = 10;
    Tester tester = opts.seed.has_value() ? Tester(opts.seed.value()) : Tester();

    tester.Test(*output_ptr, 1.f,   1.f,    test_iter_cnt);
    tester.Test(*output_ptr, 10.f,  0.1f,   test_iter_cnt);
    tester.Test(*output_ptr, 100.f, 0.01f,  test_iter_cnt);
    tester.Test(*output_ptr, 1.,    1.,     test_iter_cnt);
    tester.Test(*output_ptr, 10.,   0.1,    test_iter_cnt);
    tester.Test(*output_ptr, 100.,  0.01,   test_iter_cnt);

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