.PHONY: all configure build test run clean rebuild release test-release examples


BUILD_DIR ?= build
RELEASE_BUILD_DIR ?= build-release
BUILD_TYPE ?= Debug
PROGRAM ?= examples/fib.elf
RISCV_GCC ?= riscv64-unknown-elf-gcc
RISCV_FLAGS ?= -march=rv32i -mabi=ilp32 -static -O1
EXAMPLES := $(patsubst examples/%.c,examples/%.elf,$(wildcard examples/*.c))

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	cmake --build $(BUILD_DIR) --parallel

examples: $(EXAMPLES)

examples/%.elf: examples/%.c
	$(RISCV_GCC) $(RISCV_FLAGS) -o $@ $<

release:
	$(MAKE) build BUILD_TYPE=Release BUILD_DIR=$(RELEASE_BUILD_DIR)

test-release:
	$(MAKE) test BUILD_TYPE=Release BUILD_DIR=$(RELEASE_BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run: build
	./$(BUILD_DIR)/riscemu $(PROGRAM)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(RELEASE_BUILD_DIR)

rebuild: clean build