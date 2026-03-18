#pragma once

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <optional>
#include <array>
#include <string_view>

class CliParser {

public:

    enum class Mode {
        Chi2Export  = 0,
        TestingChi2    = 1,
        TestingRng     = 2,
        SIZE_
    };

    constexpr static std::array<std::string_view, static_cast<size_t>(Mode::SIZE_)> MODE_NAMES = {
        "Chi2Export",
        "TestChi2",
        "TestRng",
    };
    constexpr static size_t MODE_NAMES_SIZE = MODE_NAMES.size();

    struct Options {
        std::string output_file = "data.json";
        std::optional<uint64_t> seed = std::nullopt;
        uint64_t degree = 5; 
        size_t n = 10'000;
        size_t tests_cnt = 10;
        bool verbose = false;
        bool help_requested = false;
        Mode mode = Mode::Chi2Export;
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
    static size_t parseTestsCnt(const char* str, const std::string& option);
    static Mode parseMode(const char* str, const std::string& option);


private:

    Options options_;

};

