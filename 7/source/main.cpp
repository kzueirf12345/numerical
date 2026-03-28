#include <climits>
#include <cstdlib>
#include <omp.h>

#include "CliParser/CliParser.hpp"

//==========================================MAIN====================================================

int main(int argc, char* argv[]) try {
    CliParser cli(argc, argv);
    
    if (cli.shouldExit()) {
        CliParser::PrintHelp(std::cout);
        return EXIT_SUCCESS;
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