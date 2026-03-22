# Домашние задания по курсу численных методов
Попов Владимир Сергеевич, Б01-411, 2 курс ИВТ ФРКТ

## 4

### Зависимости

| Зависимость           | Минимальная версия    | Назначение                                    |
|-----------------------|-----------------------|-----------------------------------------------|
| **make**              | 4.3                   | Сборка проекта и зависимостей                 |
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
| **make**              | 4.3                   | Сборка проекта и зависимостей                 |
| **g++**               | 11.4                  | Компиляция C++20 кода                         |
| **python3**           | 3.10                  | Построение графиков                           |
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
Usage: hi_quad.out [OPTIONS]
Options:
  -m, --mode <MODE_NAME>     Specify execution mode (Chi2Export, TestChi2, TestRng) (default: Chi2Export)
  -o, --output <FILE>        Specify output file (default: data.json)
  -s, --seed <VALUE>         Specify random seed (default: random)
  -d, --degree <VALUE>       Specify degree of chi-square (default: 5)
  -n1,--n_lvl1 <VALUE>       Specify p-value count (default: 10000)
  -n2,--n_lvl2 <VALUE>       Specify iterations count for 1 p-value (default: 10000)
  -l, --lag    <VALUE>       Specify lag for autocorelation test RNG (default: 2000)  
  -t, --tests_cnt <VALUE>    Specify tests count (default: 10)
  -v, --verbose              Output exectuion progress
  -h, --help                 Show this help message
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

## 6

Это задача на тестирование генераторов случайных чисел. Мы такой писал в 5 задаче, поэтому все исходники лежат в этой же папке.

Реализовано двухуровневое тестирование на основе Колмогорова-Смирнова, а также автокорреляции. Отчёт о проведённом тестировании находится в папке ```report```.

Зависимости и использование смотреть [здесь](#5).

### Дополнительное задание (плохой ГСЧ проходящий тест на моменты)

$$
u_i = \begin{cases} 
\frac{i+1}{2n}, & \text{если } i \text{ нечётное} \\
1 - \frac{i}{2n}, & \text{если } i \text{ чётное}
\end{cases}
$$
, где n - количество чисел, которое надо сгенерировать

Мы получим равномерно-распределённые числа от 0 до 1

Будем считать, что n-чётное. При больших n вклад 1 числа рушащего симметрию буд неощутим.

Для нечётных $i = 2j-1$:
$$
u_{2j-1} = \frac{(2j-1)+1}{2n} = \frac{2j}{2n} = \frac{j}{n}
$$
Для чётных $i = 2j$:
$$
u_{2j} = 1 - \frac{2j}{2n} = 1 - \frac{j}{n}
$$

Таким образом, последовательность состоит из пар чисел вида $\left( \frac{j}{n}, 1 - \frac{j}{n} \right)$.

Заметим свойство
$$
u_{2j-1} + u_{2j} = \frac{j}{n} + \left(1 - \frac{j}{n}\right) = 1
$$

Посчитаем математическое ожидание

$$
M = \frac{1}{n}\cdot\sum_{i=1}^n u_i = \frac{1}{n}\cdot\sum_{j=1}^{n/2} (u_{2j-1} + u_{2j}) = \frac{1}{n}\cdot\sum_{j=1}^{n/2} 1 = \frac{1}{n}\cdot\frac{n}{2} = 1/2
$$

Посчитаем второй момент

$$
\sum_{i=1}^n u_i^2 = \sum_{j=1}^{n/2} \left[ \left(\frac{j}{n}\right)^2 + \left(1 - \frac{j}{n}\right)^2 \right]
$$


Раскроем скобки во втором слагаемом:
$$
\left(\frac{j}{n}\right)^2 + 1 - \frac{2j}{n} + \left(\frac{j}{n}\right)^2 = 1 - \frac{2j}{n} + \frac{2j^2}{n^2}
$$
Посчитаем суммы:
$$\sum_{j=1}^m 1 = m = \frac{n}{2}$$
$$\sum_{j=1}^m \frac{2j}{n} = \frac{2}{n} \cdot \frac{m(m+1)}{2} = \frac{1}{n} \cdot \frac{n}{2} \left(\frac{n}{2}+1\right) = \frac{1}{2} \left(\frac{n}{2}+1\right) = \frac{n}{4} + \frac{1}{2}$$
$$\sum_{j=1}^m \frac{2j^2}{n^2} = \frac{2}{n^2} \cdot \frac{m(m+1)(2m+1)}{6} = \frac{1}{3n^2} \cdot \frac{n}{2} \left(\frac{n}{2}+1\right) (n+1) = \frac{(n+2)(n+1)}{12n}$$

Собираем сумму:
$$
\sum u_i^2 = \frac{n}{2} - \left(\frac{n}{4} + \frac{1}{2}\right) + \frac{n^2+3n+2}{12n} = \frac{n}{4} - \frac{1}{2} + \frac{n}{12} + \frac{1}{4} + \frac{1}{6n}
$$
$$
= \frac{3n+n}{12} - \frac{1}{4} + \frac{1}{6n} = \frac{n}{3} - \frac{1}{4} + \frac{1}{6n}
$$
Теперь делим на $n$, чтобы получить момент $M_2$:
$$
M_2 = \frac{1}{3} - \frac{1}{4n} + \frac{1}{6n^2}
$$

Тогда можем вычислить дисперсию

$$
\sigma^2 = M_2 - M^2=\left( \frac{1}{3} - \frac{1}{4n} + \frac{1}{6n^2} \right) - \frac{1}{4} = \frac{1}{12} - \frac{1}{4n} + \frac{1}{6n^2}
$$

При увеличении выборки дисперсия стремиться к 1/12.

То есть значения мат ожидания и дисперсии совпадают с необходимыми.
