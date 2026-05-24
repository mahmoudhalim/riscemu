.PHONY: all configure build test run clean rebuild release test-release


BUILD_DIR ?= build
RELEASE_BUILD_DIR ?= build-release
BUILD_TYPE ?= Debug

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	cmake --build $(BUILD_DIR) --parallel

release:
	$(MAKE) build BUILD_TYPE=Release BUILD_DIR=$(RELEASE_BUILD_DIR)

test-release:
	$(MAKE) test BUILD_TYPE=Release BUILD_DIR=$(RELEASE_BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run: build
	./$(BUILD_DIR)/riscemu

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(RELEASE_BUILD_DIR)

rebuild: clean build
