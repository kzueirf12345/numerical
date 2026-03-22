.PHONY: all gen plot

GEN_BUILD_TYPE ?= RelWithDebInfo

all: gen plot

gen:
	cmake -B build -DCMAKE_BUILD_TYPE=$(GEN_BUILD_TYPE)
	cmake --build build -j$$(nproc)
	./build/rng $(OPTS)

plot:
	python3.10 "source/plot.py" $(PY_OPTS)