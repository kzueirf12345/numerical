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

Будем считать, что n-чётное. При больших n вклад 1 числа рушащего симметрию будет неощутим.

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

## 11 

Библиотека для замера Latency и Throughput.

### Зависимости

| Зависимость           | Минимальная версия    | Назначение                                    |
|-----------------------|-----------------------|-----------------------------------------------|
| **make**              | 4.3                   | Удобная обёртка над cmake для бенчей          |
| **g++**               | 11.4                  | Компиляция C++20 кода                         |
| **cmake**             | 3.21                  | Сборка проекта                                |

Если требуется прогнать тесты из main, а не просто подключить header, то требуется libm 2.35 и mpfr 4.1.0. Там тестируются логарифмы

### Использование

Если хотите использовать как only header библиотеку, то просто скопируйте source/Measurer/include/Measurer/Measurer.hpp к себе в проект. Описание методов будет далее.

Если хотите собрать тесты, то можно собирать при помощи cmake, тогда это будет кроссплатформенно (ну хотя бы на x86_64). Make написан для собственного удобства, он выставляет фиксированную частоту, отключается AMD_BOOST, а также фиксирует программу на одном ядре и выставляет процессу большой приоритет.

#### Cmake

```bash
$ cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
$ cmake --build $(BUILD_FOLDER) -j$(nproc)
$ ./build/measurer --help
Usage: ./build/measurer [OPTIONS]
Options:
  -m, --mode <MODE_NAME>     Specify execution mode | LATENCY |THROUGHPUT | (default: LATENCY)
  -o, --output <FILE>        Specify output file (default: stdout)
  -u, --buckets <VALUE>      Specify buckets count (default: 10)
  -a, --batches <VALUE>      Specify batches count (default: 50)
  -n, --iterations <VALUE>   Specify iterations count in bucket or in batch (default: 50000)
  -s, --seed <VALUE>         Specify seed for random (default: random)
  -v, --verbose              Output exectuion progress
  -h, --help                 Show this help message
```

#### Make

| Команда               | Назначение    
|-----------------------|-------------------------------------|
| ```make setup_cpu```  | Установить фиксированную частоту на все ядра и отключить AMD_BOOST |
| ```make restore_cpu```| включить AMD_BOOST и влючить powersave на всех ядрах |
| ```make bench```      | build + setup_cpu + запуск с большим приоритетом и на 1 ядре + restore_cpu |  

### Функционал

Всё находится в namespace measurer

#### DoNotOptimizeAway

```cpp
template <typename T>
static inline void DoNotOptimizeAway(T&& val);
```

Функция, которая принимает какое-то значение, которое вы не хотите, чтобы выкидывалось, хоть оно и не используется. Внутри пустая ассемблерная вставка, которая принимает этот аргумент и говорит компилятору, что внутри происходит работа с памятью.

#### Val

```cpp
struct Val {
    double mean; ///< среднее значение
    double stddev; ///< среднеквадратичное отклонение
};
```

#### Runner::benchLatency

```cpp
template <
  typename SetupF, ///< Тип функции подготовки данных. Не принимает аргументов.
  typename F       ///< Тип замерямой функции. Не принимает аргументов.
>
Val Runner::benchLatency(
    const size_t buckets_cnt, ///< Количесто замерямых бакетов, среди них берёться среднее значение и считает ошибка через TwoPass метод
    const size_t bucket_iterations_cnt, ///< Количество итераций в 1 бакете, среди всех итераций берётся минимальной значение, как значение с наименьшим вмешательство "шума" ОС, смены констекста и остального не относящегося к функции
    SetupF&& setup_func, ///< Функция подготовки данных, вызывается перед выполнением каждой итерации, чтобы сгенерировать какие-то необходимые входные данные, либо прогреть кэши
    F&& func ///< Замеряемая функция
)
```

Метод, для замера Latency функции. В данный метод не передаются аргументы, так как подразумевается, что setup_func и func будут лямбдами, которые будут захватывать необходимые аргументы через контекст. Вот пример использования

```cpp
double arg = 0;
std::mt19937_64 gen{seed};
std::uniform_real_distribution<double> dist(1., 1000.);

auto log = [&arg](){ 
    return std::log(arg); 
};
auto log_setup = [&arg, &gen, &dist](){ 
    static_assert(sizeof(arg) == sizeof(dist(gen)), "");
    arg = std::bit_cast<double>(dist(gen));
};

const measurer::Val log_res = measurer::Runner::benchLatency(
    buckets_cnt, iterations_cnt, log_setup, log
);
```

Также есть перегрузка без setup_func, которая из под себя вызывает эту же версию, но вместо setup_func передаёт саму замеряемую функцию, чтобы перед каждым замером прогревать кэши.

#### Runner::benchThroughput

```cpp
template <
    typename SetupF, ///< Тип функции подготовки данных. Не принимает аргументов.
    typename F       ///< Тип замерямой функции. Принимает аргумент индекса итерации, чтобы знать откуда брать данные.
>
Val Runner::benchThroughput(
    const size_t buckets_cnt, ///< Количесто замерямых бакетов, среди них берёться среднее значение и считает ошибка через TwoPass метод
    const size_t batches_cnt, ///< Количество батчей в одном бакете. Среди всех батчей берёться минимальной значение, как значение с наименьшим вмешательство "шума" ОС, смены констекста и остального не относящегося к функции
    const size_t batch_iterations_cnt, ///< Количество итераций в одном батче. В этом цикле исполняется только func и больше ничего, чтобы минизировать ошибку. Замеряется время выполнения одного батча и делиться на данное число, как среднее арифметическое. Результат получается с погрешностью на времени выполнения прыжков и инкрементации счётчика цикла, а также учитывается время выполнения последней инструкции цикла.
    SetupF&& setup_func, ///< Функция подготовки данных, вызывается перед выполнением каждого батча. Должна предоставлять для func batch_iterations_cnt входных данных
    F&& func ///< Замеряемая функция. Принимает номер текущей итерации в батче и в зависимости от этого берёт необходимые данные и проводит вычисления.
)
```

Метод, для замера Throughput функции. В данный метод не передаются аргументы, так как подразумевается, что setup_func и func будут лямбдами, которые будут захватывать необходимые аргументы через контекст. Вот пример использования

```cpp
std::vector<double> batch_args(iterations_cnt);
std::mt19937_64 gen{seed};
std::uniform_real_distribution<double> dist(1., 1000.);

auto log = [&batch_args](const size_t i){ 
    return std::log(batch_args[i]); 
};
auto log_setup = [&batch_args, &gen, &dist](){ 
    static_assert(sizeof(*batch_args.data()) == sizeof(dist(gen)), "");
    for (double& arg : batch_args) {
        arg = std::bit_cast<double>(dist(gen));
    }
};

const measurer::Val log_res = measurer::Runner::benchThroughput(
    buckets_cnt, batches_cnt, iterations_cnt, log_setup, log
);
```

Стоит отметить, что не имеет особого смысла тестировать throughput для нагруженных больших функций, так как скорее всего весь конвейер забивается в самой функции, а накладные расходы на передачу аргументов, цикл и остальное могут дать результат даже больше, чем latency. В тестах провёднных в main над mpfr как раз это и наблюдается. При этом std::log показывает корректные данные.

### Тестирование

Параметры машины, на которой проводилось тестирование

| Параметр | Значение |
| :--- | :--- |
| **Аппаратное обеспечение** | **(Hardware)** |
| Процессор (CPU) | AMD Ryzen 7 8845H (8 ядер, 16 потоков) |
| Архитектура | `x86_64` |
| Базовая / Максимальная частота | 3.8 ГГц / 5.1 ГГц |
| L1 Кэш (данные/инструкции) | 256 / 256 КиБ |
| L2 Кэш | 8 МиБ |
| L3 Кэш | 16 МиБ (общий) |
| Оперативная память | 32 ГБ LPDDR5x |
| **Программное обеспечение** | **(Software)** |
| Операционная система | Ubuntu 22.04.5 LTS |
| Ядро Linux | `6.8.0-106-generic` |
| Компилятор | GCC 11.4 / Clang 11.4 |
| Библиотека C | glibc 2.35 |
| Флаги компиляции | `-std=c++20 -O2 -g -DNDEBUG` |

---

#### Результаты

```
===Benchmarking Latency===
buckets_cnt:       10
iterations_cnt:    50000
clks_mpfr:         4434.6 +/- 44.0611
clks_libm:         152 +/- 0
mpfr_div_libm:     29.175 +/- 0.289875

===Benchmarking Throughput===
buckets_cnt:       10
batches_cnt:       50
iterations_cnt:    10000
clks_mpfr:         4859.31 +/- 133.601
clks_libm:         20.4075 +/- 0.00196231
mpfr_div_libm:     238.114 +/- 6.5467
```

Как можно видеть для std::log всё померялось очень даже хорошо, так как функция быстрая и занимает не много тактов, не полностью загружает конвейр внутри себя. При этом mpfr работает с динамическими структурами и более тяжеловесная. В значение latency поверить можно, но throughput смысла не имеет.