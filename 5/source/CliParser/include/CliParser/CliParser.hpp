#pragma once

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <optional>

class CliParser {

public:

    struct Options {
        std::string output_file = "data.json";
        std::optional<uint64_t> seed = std::nullopt;
        uint64_t degree = 5; 
        size_t n = 1'000'000;
        bool help_requested = false;
    };

public:

    CliParser(int argc, char* argv[]);

    [[nodiscard]] inline const Options& options    () const noexcept { return options_; }
    [[nodiscard]] inline bool           shouldExit () const noexcept { return options_.help_requested; }

    static void PrintHelp(std::ostream& output);

private:

    void parse(int argc, char* argv[]);

    static void checkRequireArgument(int index, int argc, const std::string& option);

    static uint64_t parseSeed(const char* str, const std::string& option);
    static uint64_t parseDegree(const char* str, const std::string& option);
    static size_t parseN(const char* str, const std::string& option);


private:

    Options options_;

};

