#include <stdexcept>

#include "CliParser/CliParser.hpp"

CliParser::CliParser(int argc, char* argv[]) {
    parse(argc, argv);
}

void CliParser::printHelp(std::ostream& output) {
    output << "Usage: ./build/call_option [OPTIONS]\n"
              "Options:\n"
              "  -o, --output <FILE>        Specify output file (default: stdout)\n"
              "  -p, --options <VALUE>      Specify options count (default: 100)\n"
              "  -n, --iterations <VALUE>   Specify iterations count (default: 100000)\n"
              "  -s, --seed <VALUE>         Specify seed for random (default: random)\n"
              "  -h, --help                 Show this help message\n";
}

void CliParser::parse(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            options_.help_requested = true;
            return;
        }
        else if (arg == "-o" || arg == "--output") {
            checkRequireArgument(i, argc, arg);
            options_.output_file = argv[++i];
        }
        else if (arg == "-n" || arg == "--iterations") {
            checkRequireArgument(i, argc, arg);
            options_.iterations_cnt = parseSize(argv[++i], arg);
        }
        else if (arg == "-p" || arg == "--options") {
            checkRequireArgument(i, argc, arg);
            options_.options_cnt = parseSize(argv[++i], arg);
        }
        else if (arg == "-s" || arg == "--seed") {
            checkRequireArgument(i, argc, arg);
            options_.seed = parseUint64(argv[++i], arg);
        }
        else if (arg.starts_with('-')) {
            throw std::invalid_argument("Unknown option: " + arg);
        }
        else {
            throw std::invalid_argument("Unexpected positional argument: " + arg);
        }
    }
}

void CliParser::checkRequireArgument(int index, int argc, const std::string& option) {
    if (index + 1 >= argc) {
        throw std::invalid_argument("Option " + option + " requires an argument");
    }
}

uint64_t CliParser::parseUint64(const char* str, const std::string& option) try {
    size_t pos = 0;

    unsigned long val = std::stoul(str, &pos);
    if (pos != std::strlen(str)) {
        throw std::invalid_argument("");
    }

    return static_cast<uint64_t>(val);
}
catch (...) {
    throw std::invalid_argument("Invalid uint64_t value for " + option + ": " + str);
}

size_t CliParser::parseSize(const char* str, const std::string& option) try {
    size_t pos = 0;

    unsigned long val = std::stoul(str, &pos);
    if (pos != std::strlen(str)) {
        throw std::invalid_argument("");
    }

    return static_cast<size_t>(val);
}
catch (...) {
    throw std::invalid_argument("Invalid size_t value for " + option + ": " + str);
}

double CliParser::parseDouble(const char* str, const std::string& option) try {
    size_t pos = 0;

    double val = std::stod(str, &pos);
    if (pos != std::strlen(str)) {
        throw std::invalid_argument("");
    }

    return static_cast<double>(val);
}
catch (...) {
    throw std::invalid_argument("Invalid size_t value for " + option + ": " + str);
}