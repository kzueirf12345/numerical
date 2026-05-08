#pragma once

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>

class CliParser {

public:

    struct Options {
        std::string output_file = "";
        size_t iterations_cnt = 100000;
        size_t options_cnt = 112;
        uint64_t seed = std::random_device{}();
        bool help_requested = false;
    };

public:

    CliParser(int argc, char* argv[]);

    [[nodiscard]] inline const Options& options    () const noexcept { return options_; }
    [[nodiscard]] inline bool           shouldExit () const noexcept { return options_.help_requested; }

    static void printHelp(std::ostream& output);

private:

    void parse(int argc, char* argv[]);

    static void checkRequireArgument(int index, int argc, const std::string& option);

    static uint64_t parseUint64(const char* str, const std::string& option);
    static size_t parseSize(const char* str, const std::string& option);
    static double parseDouble(const char* str, const std::string& option);

private:

    Options options_;

};