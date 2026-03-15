# Домашние задания по курсу численных методов
Попов Владимир Сергеевич, Б01-411, 2 курс ИВТ ФРКТ

## 4

### Зависимости

| Зависимость           | Минимальная версия    | Назначение                                    |
|-----------------------|-----------------------|-----------------------------------------------|
| **Make**              | 4.3                   | Сборка проекта и зависимостей                 |
| **g++**               | 11.4                  | Компиляция C++20 кода                         |

### Использование

| Команда               | Назначение    
|-----------------------|-------------------------------------|
| ```make build```      | Собирает проект                     |
| ```make clean```      | Очищает папку build и логи          | 
| ```make rebuild```    | clean + build                       |
| ```make start```      | Запускает проект                    | 
| ```make all```        | build + start                       | 

Опции можно передать через аргумент ```OPTS``` в двойных ковычках, либо же напрямую исполняемому файлу. Для просмотра всех возможных опций в качестве аргумента передайте -h или --help.

```bash
$ make OPTS="--help"
./cat_ost_roof.out --help
Usage: cat_ast_roof.out [OPTIONS]
Options:
  -o, --output <FILE>    Specify output file (default: stdout)
  -s, --seed <VALUE>     Specify random seed (default: random)
  -h, --help             Show this help message
```

## 5 

### Зависимости

| Зависимость           | Минимальная версия    | Назначение                                    |
|-----------------------|-----------------------|-----------------------------------------------|
| **Make**              | 4.3                   | Сборка проекта и зависимостей                 |
| **g++**               | 11.4                  | Компиляция C++20 кода                         |
| **Python3**           | 3.10                  | Построение графиков                           |
| **numpy**             | 2.2.6                 | Построение графиков                           |
| **matplotlib**        | 3.10.6                | Построение графиков                           |
| **scipy**             | 1.15.3                | Построение графиков                           |

### Использование

| Команда               | Назначение    
|-----------------------|-------------------------------------|
| ```make build```      | Собирает $\chi^2$ генератор         |
| ```make clean```      | Очищает папку build и логи          | 
| ```make rebuild```    | clean + build                       |
| ```make start```      | Запускает $\chi^2$ генератор        | 
| ```make gen```        | build + start                       | 
| ```make plot```       | Запускает скрипт построения графика | 
| ```make all```        | gen + plot                          | 

Опции генератору можно передать через аргумент ```OPTS``` в двойных ковычках, а опции питоновскому скрипту для построения графиков через аргумент ```PY_OPTS```. Для просмотра всех возможных опций в качестве аргумента передайте -h или --help.

```bash
$ make gen OPTS="--help"
./hi_quad.out --help
Usage: hi_quad.out [OPTIONS]
Options:
  -o, --output <FILE>       Specify output file (default: data.json)
  -s, --seed <VALUE>        Specify random seed (default: random)
  -d, --degree <VALUE>      Specify degree of chi-square (default: 5)
  -n, --iters_cnt <VALUE>   Specify iterations count (default: 1000000)
  -h, --help                Show this help message
```
```bash
$ make plot PY_OPTS="--help"
python3.10 "./src/plot.py" --help
usage: plot.py [-h] [-o OUTPUT] [-b BINS] [-q] input

Визуализация генератора хи-квадрат

positional arguments:
  input                       Путь к входному JSON-файлу

options:
  -h, --help                  show this help message and exit
  -o OUTPUT, --output OUTPUT  Путь для сохранения графика (по умолчанию: показать на экране)
  -b BINS, --bins BINS        Количество бинов гистограммы (автоподбор по умолчанию)
  -q, --quiet                 Не выводить статистику в консоль
```

Пример генерации

```bash
$ make OPTS="--degree 100" PY_OPTS="data.json -o plot_n100.png"
```

Полученные графики для некоторых параметров можно посмотреть в папке ```examples```

### Дополнительное задание (связь нормального и логнормального распределений)

$$
\begin{aligned}
F_{LN}(x, a, \sigma)  &= F_{N}(\ln x, a, \sigma) - \text{функции распределения}                               \\
                      &= \int_{-\infty}^{\ln x} \frac{1}{\sigma\sqrt{2\pi}} e^{-\frac{(t-a)^2}{2\sigma^2}} dt \\
                      &\quad \left| \begin{matrix} 
                          t = \ln y                                                                           \\ 
                          dt = \frac{dy}{y}                                                                   \\ 
                          -\infty \to 0                                                                       \\ 
                          \ln x \to x 
                      \end{matrix} \right.                                                                    \\
                      &= \int_{0}^{x} \frac{1}{y\sigma\sqrt{2\pi}} e^{-\frac{(\ln y - a)^2}{2\sigma^2}} dy
\end{aligned}
$$