# Домашние задания по курсу численных методов
Попов Владимир Сергеевич, Б01-411, 2 курс ИВТ ФРКТ

## 4
Для запуска программы необходимо перейти в папку 4 и запустить Makefile. Необходимо установить make и g++ с поддержкой 20-ых плюсов.


Чтобы только собрать проект можно использовать команду ```make build```, чтобы пересобрать - ```make rebuild```, чтобы запустить - ```make start```.

Опции можно передать через аргумент OPTS в двойных ковычках, либо же напрямую исполняемому файлу. Для просмотра всех возможных опций в качестве аргумента передайте -h или --help.

```bash
# pwd = .../numerical/
$ cd ./4
$ make OPTS="--help"
./cat_ost_roof.out --help
Usage: cat_ast_roof.out [OPTIONS]
Options:
  -o, --output <FILE>    Specify output file (default: stdout)
  -s, --seed <VALUE>     Specify random seed (default: random)
  -h, --help             Show this help message
```