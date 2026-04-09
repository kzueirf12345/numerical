.PHONY: all build clean rebuild \
        libs_build libs_clean libs_rebuild \
		clean_all clean_log clean_out clean_obj clean_deps clean_txt clean_bin

PROJECT_NAME = matmul

BUILD_DIR = ./build
SRC_DIR = ./src
COMPILER = gcc

DEBUG_ ?= 1

ifeq ($(origin FLAGS), undefined)

FLAGS =	-Wall -Wextra -Waggressive-loop-optimizations \
		-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
		-Wconversion -Wempty-body -Wfloat-equal \
		-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
		-Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self \
		-Wredundant-decls -Wshadow -Wsign-conversion \
		-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
		-Wsuggest-final-types -Wswitch-default -Wswitch-enum -Wsync-nand \
		-Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
		-Wno-missing-field-initializers -Wno-narrowing -Wno-varargs \
		-Wstack-protector -fcheck-new -fstack-protector -fstrict-overflow \
		-fno-omit-frame-pointer -Wlarger-than=81920 -Wstack-usage=81920 -pie \
		-fPIE -Werror=vla

SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,$\
		integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,$\
		shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -static-libasan

DEBUG_FLAGS = -D _DEBUG  -ggdb -Og -g3 -D_FORTIFY_SOURCES=3 $(SANITIZER)
RELEASE_FLAGS = -DNDEBUG -O3

ifneq ($(DEBUG_),0)
FLAGS += $(DEBUG_FLAGS)
else
FLAGS += $(RELEASE_FLAGS)
endif

endif

FLAGS += $(ADD_FLAGS)

LIBS = -lm -L./libs/logger -llogger 


DIRS = flags utils matrix bench
BUILD_DIRS = $(DIRS:%=$(BUILD_DIR)/%)

SOURCES = main.c flags/flags.c utils/utils.c matrix/funcs.c bench/bench.c

SOURCES_REL_PATH = $(SOURCES:%=$(SRC_DIR)/%)
OBJECTS_REL_PATH = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPS_REL_PATH = $(OBJECTS_REL_PATH:%.o=%.d)


all: build start

start:
	./$(PROJECT_NAME).out $(OPTS)

build: $(PROJECT_NAME).out

rebuild: clean_all build


$(PROJECT_NAME).out: $(OBJECTS_REL_PATH)
	@$(COMPILER) $(FLAGS) -o $@ $^  $(LIBS)

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c | ./$(BUILD_DIR)/ $(BUILD_DIRS) libs_build
	@$(COMPILER) $(FLAGS) -I$(SRC_DIR) -I./libs -c -MMD -MP $< -o $@

-include $(DEPS_REL_PATH)

$(BUILD_DIRS):
	mkdir $@
./$(BUILD_DIR)/:
	mkdir $@

libs_rebuild: libs_clean libs_build

libs_build:
	@make ADD_FLAGS="$(ADD_FLAGS)" FLAGS="$(FLAGS)" DEBUG_=$(DEBUG_) build -C ./libs/logger/

libs_clean:
	make clean -C ./libs/logger/


clean_all: clean libs_clean

clean: clean_obj clean_deps clean_out libs_clean

clean_log:
	rm -rf ./log/*

clean_out:
	rm -rf ./*.out

clean_obj:
	rm -rf ./$(OBJECTS_REL_PATH)

clean_deps:
	rm -rf ./$(DEPS_REL_PATH)

clean_txt:
	rm -rf ./*.txt

clean_bin:
	rm -rf ./*.bin


# -ffp-contract=off для отключения fma
BENCH_FLAGS = -march=native

CPU_CORE = 0
GOVERNOR = performance
AMD_BOOST = /sys/devices/system/cpu/cpufreq/boost
FIXED_FREQ = 3000MHz 

.PHONY: bench setup_cpu restore_cpu

bench: clean_out
	$(MAKE) DEBUG_=0 ADD_FLAGS="$(BENCH_FLAGS)" rebuild
	@$(MAKE) setup_cpu
	sudo taskset -c $(CPU_CORE) chrt -f 99 ./$(PROJECT_NAME).out $(OPTS)
	@$(MAKE) restore_cpu

setup_cpu:
	sudo cpupower frequency-set -u $(FIXED_FREQ) -d $(FIXED_FREQ) -g $(GOVERNOR)
	if [ -f $(AMD_BOOST) ]; then echo 0 | sudo tee $(AMD_BOOST); fi

restore_cpu:
	sudo cpupower frequency-set -g powersave
	if [ -f $(AMD_BOOST) ]; then echo 1 | sudo tee $(AMD_BOOST); fi