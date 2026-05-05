#include <stdexcept>

#include "CliParser/CliParser.hpp"

CliParser::CliParser(int argc, char* argv[]) {
    parse(argc, argv);
}

void CliParser::printHelp(std::ostream& output) {
    output << "Usage: ./build/call_option [OPTIONS]\n"
              "Options:\n"
              "  -m, --mode <MODE_NAME>     Specify execution mode | "
              
#define DEFINE_ENUM_(name) << #name << " |"
        MODE_LIST_(DEFINE_ENUM_)
#undef DEFINE_ENUM_
              
           << " (default: LATENCY)\n"
              "  -o, --output <FILE>        Specify output file (default: stdout)\n"
              "  -c, --start-cost <VALUE>   Specify start cost (default: 100)\n"
              "  -r, --rate <VALUE>         Specify rate (default: 0.05)\n"
              "  -v, --volatility <VALUE>   Specify volatility (default: 0.1)\n"
              "  -t, --strike <VALUE>       Specify strike (default: 100)\n"
              "  -p, --period <VALUE>       Specify period (default: 1)\n"
              "  -n, --iterations <VALUE>   Specify iterations count (default: 1000000)\n"
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
        else if (arg == "-c" || arg == "--start-cost") {
            checkRequireArgument(i, argc, arg);
            options_.start_cost = parseDouble(argv[++i], "--start-cost");
        }
        else if (arg == "-r" || arg == "--rate") {
            checkRequireArgument(i, argc, arg);
            options_.rate = parseDouble(argv[++i], "--rate");
        }
        else if (arg == "-v" || arg == "--volatility") {
            checkRequireArgument(i, argc, arg);
            options_.volatility = parseDouble(argv[++i], "--volatility");
        }
        else if (arg == "-t" || arg == "--strike") {
            checkRequireArgument(i, argc, arg);
            options_.strike = parseDouble(argv[++i], "--strike");
        }
        else if (arg == "-p" || arg == "--period") {
            checkRequireArgument(i, argc, arg);
            options_.period = parseDouble(argv[++i], "--period");
        }
        else if (arg == "-m" || arg == "--mode") {
            checkRequireArgument(i, argc, arg);
            options_.mode = parseMode(argv[++i], "--mode");
        }
        else if (arg == "-n" || arg == "--iterations") {
            checkRequireArgument(i, argc, arg);
            options_.iterations_cnt = parseSize(argv[++i], "--iterations");
        }
        else if (arg == "-s" || arg == "--seed") {
            checkRequireArgument(i, argc, arg);
            options_.seed = parseUint64(argv[++i], "--seed");
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

CliParser::Mode CliParser::parseMode(const char* str, const std::string& option) try {
    return mode_str2enum_map.at(str);
}
catch (...) {
    throw std::invalid_argument("Invalid mode for " + option + ": " + str);
}