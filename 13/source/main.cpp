#include <climits>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <sleef.h>
#include <stdexcept>
#include <mpfr.h>
#include <x86intrin.h>

#include "CliParser/CliParser.hpp"
#include "rialto/rialto.hpp"

int main(int argc, char* argv[]) try {
    CliParser cli(argc, argv);
    
    if (cli.shouldExit()) {
        CliParser::printHelp(std::cout);
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

    const rialto::Rialto::Context context{
        .start_cost = opts.start_cost,
        .rate = opts.rate,
        .volatility = opts.volatility,
        .strike = opts.strike,
        .period = opts.period,
    };

    rialto::Rialto rialto(std::move(context));

    const double monte_carlo_cost = rialto.MonteCarlo(opts.iterations_cnt, opts.seed);
    const double analytic_cost = rialto.BlackScholes();

    const double abs_error = std::abs(monte_carlo_cost - analytic_cost);
    const double rel_error_pct = (abs_error / analytic_cost * 100.0);

    *output_ptr << std::fixed << std::setprecision(5) <<
        "===ОТЧЁТ===\n"
        "[Параметры рынка и контракта]\n"
        "  Начальная цена (start_cost)          : " << opts.start_cost << "\n"
        "  Страйк (strike)                      : " << opts.strike << "\n"
        "  Безрисковая ставка (rate)            : " << opts.rate << "\n"
        "  Волатильность (volatility)           : " << opts.volatility << "\n"
        "  Время (period)                       : " << opts.period << "\n"
        "\n[Параметры симуляции Монте-Карло]\n"
        "  Количество итераций (iterations_cnt) : " << opts.iterations_cnt << "\n"
        "  Сид генератора (seed)                : " << opts.seed << "\n"
        "\n[Результаты вычислений]\n"
        "  Monte-Carlo cost                     : " << monte_carlo_cost << "\n"
        "  Analytic cost                        : " << analytic_cost << "\n"
        "  Абсолютная разница                   : " << abs_error << "\n"
        "  Относительная погрешность            : " << rel_error_pct << "%\n"
    << std::endl;

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