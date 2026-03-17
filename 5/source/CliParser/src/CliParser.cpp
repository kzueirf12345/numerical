#include "CliParser/CliParser.hpp"

CliParser::CliParser(int argc, char* argv[]) {
    parse(argc, argv);
}

void CliParser::PrintHelp(std::ostream& output) {
    output << "Usage: hi_quad.out [OPTIONS]\n"
              "Options:\n"
              "  -o, --output <FILE>        Specify output file (default: data.json)\n"
              "  -s, --seed <VALUE>         Specify random seed (default: random)\n"
              "  -d, --degree <VALUE>       Specify degree of chi-square (default: 5)\n"
              "  -n, --iters_cnt <VALUE>    Specify iterations count (default: 1'000'000)\n"
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
            options_.seed = parseSeed(argv[++i], arg);
        }
        else if (arg == "-d" || arg == "--degree") {
            checkRequireArgument(i, argc, arg);
            options_.degree = parseDegree(argv[++i], arg);
        }
        else if (arg == "-n" || arg == "--iters_cnt") {
            checkRequireArgument(i, argc, arg);
            options_.n = parseN(argv[++i], arg);
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

uint64_t CliParser::parseSeed(const char* str, const std::string& option) try {
    size_t pos = 0;

    unsigned long val = std::stoul(str, &pos);
    if (pos != std::strlen(str)) {
        throw std::invalid_argument("");
    }

    return static_cast<uint64_t>(val);
}
catch (...) {
    throw std::invalid_argument("Invalid seed value for " + option + ": " + str);
}

uint64_t CliParser::parseDegree(const char* str, const std::string& option) try {
    size_t pos = 0;

    unsigned long val = std::stoul(str, &pos);
    if (pos != std::strlen(str)) {
        throw std::invalid_argument("");
    }

    return static_cast<uint64_t>(val);
}
catch (...) {
    throw std::invalid_argument("Invalid degree value for " + option + ": " + str);
}

size_t CliParser::parseN(const char* str, const std::string& option) try {
    size_t pos = 0;

    unsigned long val = std::stoul(str, &pos);
    if (pos != std::strlen(str)) {
        throw std::invalid_argument("");
    }

    return static_cast<size_t>(val);
}
catch (...) {
    throw std::invalid_argument("Invalid iterations count value for " + option + ": " + str);
}