#include "CliParser/CliParser.hpp"
#include <stdexcept>

CliParser::CliParser(int argc, char* argv[]) {
    parse(argc, argv);
}

void CliParser::PrintHelp(std::ostream& output) {
    output << "Usage: hi_quad.out [OPTIONS]\n"
              "Options:\n"
              "  -m, --mode <MODE_NAME>     Specify execution mode (";

    for (size_t mode_ind = 0; mode_ind < MODE_NAMES_SIZE; ++mode_ind) {
        output << MODE_NAMES[mode_ind];
        if (mode_ind + 1 != MODE_NAMES_SIZE) {
            output << ", ";
        }
    }
              
    output << ") (default: Chi2Export)\n"
              "  -o, --output <FILE>        Specify output file (default: data.json)\n"
              "  -s, --seed <VALUE>         Specify random seed (default: random)\n"
              "  -d, --degree <VALUE>       Specify degree of chi-square (default: 5)\n"
              "  -n1,--n_lvl1 <VALUE>       Specify p-value count (default: 10'000)\n"
              "  -n2,--n_lvl2 <VALUE>       Specify iterations count for 1 p-value (default: 10'000)\n"
              "  -l, --lag    <VALUE>       Specify lag for autocorelation test RNG (default: 2000)\n"
              "  -t, --tests_cnt <VALUE>    Specify tests count (default: 10)\n"
              "  -v, --verbose              Output exectuion progress\n"
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
        else if (arg == "-s" || arg == "--seed") {
            checkRequireArgument(i, argc, arg);
            options_.seed = parseUint64(argv[++i], arg);
        }
        else if (arg == "-d" || arg == "--degree") {
            checkRequireArgument(i, argc, arg);
            options_.degree = parseUint64(argv[++i], arg);
        }
        else if (arg == "-n2" || arg == "--n_lvl2") {
            checkRequireArgument(i, argc, arg);
            options_.n_lvl2 = parseSize(argv[++i], arg);
        }
        else if (arg == "-n1" || arg == "--n_lvl1") {
            checkRequireArgument(i, argc, arg);
            options_.n_lvl1 = parseSize(argv[++i], arg);
        }
        else if (arg == "-l" || arg == "--lag") {
            checkRequireArgument(i, argc, arg);
            options_.lag = parseSize(argv[++i], arg);
        }
        else if (arg == "-t" || arg == "--tests_cnt") {
            checkRequireArgument(i, argc, arg);
            options_.tests_cnt = parseSize(argv[++i], arg);
        }
        else if (arg == "-v" || arg == "--verbose") {
            options_.verbose = true;
        }
        else if (arg == "-m" || arg == "--mode") {
            checkRequireArgument(i, argc, arg);
            options_.mode = parseMode(argv[++i], arg);
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

CliParser::Mode CliParser::parseMode(const char* str, const std::string& option) try {
    Mode mode = Mode::SIZE_;

    for (size_t cur_mode_ind = 0; cur_mode_ind < MODE_NAMES_SIZE; ++cur_mode_ind) {
        if (MODE_NAMES[cur_mode_ind] == std::string_view(str)) {
            mode = static_cast<Mode>(cur_mode_ind);
        }
    }

    if (mode == Mode::SIZE_) {
        throw std::invalid_argument("");
    }

    return mode;
}
catch (...) {
    throw std::invalid_argument("Invalid mode value for " + option + ": " + str);
}