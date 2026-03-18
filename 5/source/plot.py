import json
import numpy as np
import matplotlib.pyplot as plt
import scipy
from scipy.stats import chi2
from pathlib import Path
import argparse

def load_chi2_data(filepath: str) -> tuple[np.ndarray, int, int]:
    with open(filepath, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    samples = np.array(data['value'], dtype=np.float64)
    n_dof = int(data['degree'])
    seed = int(data["seed"])
    
    return samples, n_dof, seed


def compute_statistics(samples: np.ndarray, n_dof: int) -> dict:
    mean_emp = np.mean(samples)
    var_emp = np.var(samples, ddof=0)
    
    mean_theor = n_dof
    var_theor = 2 * n_dof
    
    return {
        'mean_emp': mean_emp,
        'var_emp': var_emp,
        'mean_theor': mean_theor,
        'var_theor': var_theor,
        'mean_err_pct': abs(mean_emp - mean_theor) / mean_theor * 100,
        'var_err_pct': abs(var_emp - var_theor) / var_theor * 100,
    }


def plot_comparison(
    samples: np.ndarray,
    n_dof: int,
    seed: int,
    output_path: str = None,
    n_bins: int = None
):
    if n_bins is None:
        n_bins = min(100, max(30, int(np.sqrt(len(samples)))))
    
    x_max = max(samples)
    x = np.linspace(0, x_max, 500)
    
    pdf_theor = chi2.pdf(x, df=n_dof)
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    ax.hist(
        samples,
        bins=n_bins,
        density=True,
        alpha=0.65,
        label='Эмпирическое распределение',
        edgecolor='black',
        linewidth=0.5,
        color='steelblue'
    )
    
    ax.plot(
        x, pdf_theor,
        'r-',
        linewidth=2.5,
        label=f'Теоретическая плотность $\\chi^2$({n_dof})'
    )
    
    ax.set_xlabel('x', fontsize=12)
    ax.set_ylabel('Плотность вероятности', fontsize=12)
    title = f'Сравнение генератора $\\chi^2$({n_dof})'
    ax.set_title(title, fontsize=14, pad=15)
    ax.legend(fontsize=10, framealpha=0.9)
    ax.grid(alpha=0.3, linestyle='--')
    ax.set_axisbelow(True)
    
    stats = compute_statistics(samples, n_dof)
    stats_text = (
        f'Seed = {seed}\n'
        f'N = {len(samples):,}\n'
        f'Среднее: {stats["mean_emp"]:.3f} (теор. {stats["mean_theor"]})\n'
        f'Дисперсия: {stats["var_emp"]:.3f} (теор. {stats["var_theor"]})\n'
        f'Ошибка среднего: {stats["mean_err_pct"]:.2f}%\n'
        f'Ошибка дисперсии: {stats["var_err_pct"]:.2f}%'
    )
    
    props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)
    ax.text(
        0.98, 0.98, stats_text,
        transform=ax.transAxes,
        fontsize=9,
        verticalalignment='top',
        horizontalalignment='right',
        bbox=props
    )
    
    plt.tight_layout()
    
    if output_path:
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"График сохранён: {output_path}")
    else:
        plt.show()
    
    return fig, ax


def print_validation_report(samples: np.ndarray, n_dof: int):
    stats = compute_statistics(samples, n_dof)
    
    print(f"\n{'='*60}")
    print(f"Статистики хи-квадрат({n_dof})")
    print(f"{'='*60}")
    print(f"  Объём выборки:     {len(samples):,}")
    print(f"  Среднее:           {stats['mean_emp']:.5f}  (теор.: {stats['mean_theor']:.1f})")
    print(f"  Дисперсия:         {stats['var_emp']:.5f}  (теор.: {stats['var_theor']:.1f})")
    print(f"Относительные ошибки:")
    print(f"  Среднее:           {stats['mean_err_pct']:.3f}%")
    print(f"  Дисперсия:         {stats['var_err_pct']:.3f}%\n")


def main():
    parser = argparse.ArgumentParser(
        description='Визуализация генератора хи-квадрат'
    )
    parser.add_argument(
        'input',
        type=str,
        help='Путь к входному JSON-файлу'
    )
    parser.add_argument(
        '-o', '--output',
        type=str,
        default=None,
        help='Путь для сохранения графика (по умолчанию: показать на экране)'
    )
    parser.add_argument(
        '-b', '--bins',
        type=int,
        default=None,
        help='Количество бинов гистограммы (автоподбор по умолчанию)'
    )
    parser.add_argument(
        '-q', '--quiet',
        action='store_true',
        help='Не выводить статистику в консоль'
    )
    
    args = parser.parse_args()
    
    if not Path(args.input).exists():
        print(f"Файл не найден: {args.input}")
        return 1
    
    
    print(f"Загрузка данных из {args.input}...")
    samples, n_dof, seed = load_chi2_data(args.input)
    print(f"Найдено {len(samples):,} образцов, степени свободы: {n_dof}. Seed = {seed}")
    
    if not args.quiet:
        print_validation_report(samples, n_dof)
    
    print("Построение графика...")
    plot_comparison(
        samples=samples,
        n_dof=n_dof,
        seed=seed,
        output_path=args.output,
        n_bins=args.bins
    )

    
    return 0


if __name__ == '__main__':
    exit(main())