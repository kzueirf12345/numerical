#pragma once

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>

class CliParser {

public:

#define MODE_LIST_(V) \
    V(BASE)        \

#define DEFINE_ENUM_(name) name,
    enum class Mode {
        MODE_LIST_(DEFINE_ENUM_)
    };
#undef DEFINE_ENUM_

#define DEFINE_ENUM_(name) {Mode::name, #name},
    static inline const std::unordered_map<enum Mode, std::string_view> mode_enum2str_map {
        MODE_LIST_(DEFINE_ENUM_)
    };
#undef DEFINE_ENUM_

#define DEFINE_ENUM_(name) {#name, Mode::name},
    static inline const std::unordered_map<std::string_view, enum Mode> mode_str2enum_map {
        MODE_LIST_(DEFINE_ENUM_)
    };
#undef DEFINE_ENUM_

public:

    struct Options {
        std::string output_file = "";
        size_t iterations_cnt = 1000000;
        double start_cost = 100;
        double rate = 0.05;
        double volatility = 0.1;
        double strike = 100;
        double period = 1;
        Mode mode = Mode::BASE;
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
    static Mode parseMode(const char* str, const std::string& option);

private:

    Options options_;

};