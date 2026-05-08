#include <climits>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <sleef.h>
#include <stdexcept>
#include <mpfr.h>
#include <thread>
#include <x86intrin.h>
#include <algorithm>

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

    std::mt19937 params_gen(opts.seed);

    std::uniform_real_distribution<float> dist_s0(50.0, 150.0);
    std::uniform_real_distribution<float> dist_k(50.0, 150.0);
    std::uniform_real_distribution<float> dist_r(0.01, 0.10);
    std::uniform_real_distribution<float> dist_vol(0.1, 0.5);
    std::uniform_real_distribution<float> dist_t(0.1, 3.0);

    rialto::BatchContext ctx;
    ctx.count = opts.options_cnt;
    ctx.start_cost.resize(ctx.count);
    ctx.strike.resize(ctx.count);
    ctx.rate.resize(ctx.count);
    ctx.volatility.resize(ctx.count);
    ctx.period.resize(ctx.count);

    for (size_t i = 0; i < ctx.count; ++i) {
        ctx.start_cost[i] = dist_s0(params_gen);
        ctx.strike[i]     = dist_k(params_gen);
        ctx.rate[i]       = dist_r(params_gen);
        ctx.volatility[i] = dist_vol(params_gen);
        ctx.period[i]     = dist_t(params_gen);
    }

    auto monte_carlo_costs = rialto::Pricer::MonteCarlo(ctx, opts.iterations_cnt, opts.seed);
    auto analytic_costs    = rialto::Pricer::BlackScholes(ctx);

    //remove zeros for good statistic
    for (size_t i = monte_carlo_costs.size() - 1; i-->0;) {
        if (monte_carlo_costs[i] == 0.0f || analytic_costs[i] == 0.0f) {
            monte_carlo_costs.erase(monte_carlo_costs.begin() + i);
            analytic_costs.erase(analytic_costs.begin() + i);
        }
    }

    ctx.count = monte_carlo_costs.size();

    double max_abs_error = 0.0;
    double sum_abs_error = 0.0;
    double sum_rel_error = 0.0;

    for (size_t i = 0; i < ctx.count; ++i) {
        const double mc_cost = monte_carlo_costs[i];
        const double bs_cost = analytic_costs[i];

        const double abs_error = std::abs(mc_cost - bs_cost);
        const double rel_error = abs_error / bs_cost;

        max_abs_error = std::max(max_abs_error, abs_error);
        sum_abs_error += abs_error;
        sum_rel_error += rel_error;
    }

    const double mean_abs_error = sum_abs_error / ctx.count;
    const double mean_rel_error = sum_rel_error / ctx.count;

    *output_ptr << std::fixed << std::setprecision(5) 
        <<  "===ОТЧЁТ===\n"
            "[Параметры батча]\n"
            "  Количество опционов                  : " << ctx.count << "\n"
            "  Сид генерации                        : " << opts.seed << "\n"
            "\n[Параметры симуляции Монте-Карло]\n"
            "  Количество итераций на опцион        : " << opts.iterations_cnt << "\n"
            "  Потоков процессора                   : " << std::thread::hardware_concurrency() << "\n"
            "\n[Агрегированные результаты ошибок]\n"
            "  Максимальная абсолютная разница      : " << max_abs_error << "\n"
            "  Средняя абсолютная разница           : " << mean_abs_error << "\n"
            "  Средняя относительная ошибка         : " << (mean_rel_error * 100.0) << "%\n\n"
            "[Результаты " <<  ctx.count << " опционов]\n"
        << std::left 
        << std::setw(4)  << "ID" 
        << std::setw(12) << "start_cost" 
        << std::setw(12) << "strike" 
        << std::setw(12) << "rate"
        << std::setw(12) << "volatility"
        << std::setw(12) << "period"
        << std::setw(12) << "MC Cost" 
        << std::setw(12) << "BS Cost" 
        << std::setw(12) << "Rel Error" 
    << "\n--------------------------------------------------------------\n";

    for (size_t i = 0; i < ctx.count; ++i) {
        const double abs_err = std::abs(monte_carlo_costs[i] - analytic_costs[i]);
        const double rel_error_pct = (abs_err / analytic_costs[i] * 100.0);

        *output_ptr << std::left 
            << std::setw(4)  << i 
            << std::setw(12) << ctx.start_cost[i] 
            << std::setw(12) << ctx.strike[i] 
            << std::setw(12) << ctx.rate[i]
            << std::setw(12) << ctx.volatility[i]
            << std::setw(12) << ctx.period[i]
            << std::setw(12) << monte_carlo_costs[i] 
            << std::setw(12) << analytic_costs[i] 
            << std::setw(12) << rel_error_pct
        << "\n";
    }

    *output_ptr << "=============================================\n" << std::endl;

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