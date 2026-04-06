.PHONY: all build clean rebuild \
        libs_build libs_clean libs_rebuild \
		clean_all clean_log clean_out clean_obj clean_deps clean_txt clean_bin

PROJECT_NAME = vogf

BUILD_DIR = ./build
SRC_DIR = ./src
COMPILER = gcc

DEBUG_ ?= 1

ifeq ($(origin FLAGS), undefined)

FLAGS =	-Wall -Wextra -Waggressive-loop-optimizations \
		-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
		-Wconversion -Wempty-body \
		-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
		-Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self \
		-Wredundant-decls -Wshadow -Wsign-conversion \
		-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
		-Wsuggest-final-types -Wswitch-default -Wswitch-enum -Wsync-nand \
		-Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
		-Wno-missing-field-initializers -Wno-narrowing -Wno-varargs \
		-Wstack-protector -fcheck-new -fstack-protector -fstrict-overflow \
		-fno-omit-frame-pointer -pie \
		-fPIE -Werror=vla \

# SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,$\
# 		integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,$\
# 		shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -static-libasan

DEBUG_FLAGS = -D _DEBUG  -ggdb -Og -g3 -D_FORTIFY_SOURCES=3 $(SANITIZER)
RELEASE_FLAGS = -DNDEBUG -O2 -march=native

ifneq ($(DEBUG_),0)
FLAGS += $(DEBUG_FLAGS)
else
FLAGS += $(RELEASE_FLAGS)
endif

endif

FLAGS += -fopenmp $(ADD_FLAGS)

LIBS = -lm -lmpfr -L./libs/logger -llogger 


DIRS = flags utils vogf testing
BUILD_DIRS = $(DIRS:%=$(BUILD_DIR)/%)

SOURCES = main.c flags/flags.c utils/utils.c vogf/vogf.c testing/testing.c

SOURCES_REL_PATH = $(SOURCES:%=$(SRC_DIR)/%)
OBJECTS_REL_PATH = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPS_REL_PATH = $(OBJECTS_REL_PATH:%.o=%.d)


all: build start

start:
	./$(PROJECT_NAME).out $(OPTS)

build: $(PROJECT_NAME).out

rebuild: clean_all build

tablegen:
	$(COMPILER) $(SRC_DIR)/tablegen.c -o tablegen.out -lmpfr;
	./tablegen.out > $(SRC_DIR)/vogf/tables.h


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


clean_all: clean libs_clean clean_pgo

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



PGO_DIR = $(BUILD_DIR)/pgo_data
PGO_GEN_FLAGS = -fprofile-generate=$(PGO_DIR)
PGO_USE_FLAGS = -fprofile-use=$(PGO_DIR) -fprofile-correction

.PHONY: pgo

pgo: clean_all
	@mkdir -p $(PGO_DIR)
	@echo "--- Phase 1: Building with instrumentation ---"
	$(MAKE) build ADD_FLAGS="$(PGO_GEN_FLAGS)" DEBUG_=0
	
	@echo "--- Phase 2: Running training (Exhaustive test) ---"
	./$(PROJECT_NAME).out $(OPTS)
	
	@echo "--- Phase 3: Rebuilding with profile data ---"
	$(MAKE) clean_obj clean_deps clean_out
	$(MAKE) build ADD_FLAGS="$(PGO_USE_FLAGS)" DEBUG_=0
	
	@echo "--- PGO Build Complete ---"

clean_pgo:
	rm -rf $(PGO_DIR)