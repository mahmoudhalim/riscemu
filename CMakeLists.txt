cmake_minimum_required(VERSION 3.10)

project(riscemu CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

add_library(riscv-core STATIC
  ./src/cpu.cpp
  ./src/instructions.cpp
)

add_executable(riscemu ./src/main.cpp)
target_link_libraries(riscemu PRIVATE riscv-core)
