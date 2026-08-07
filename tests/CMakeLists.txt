add_executable(unit_tests
    cpu_test.cpp
    decoder_test.cpp
    executor_itype_test.cpp
    executor_jtype_test.cpp
    executor_rtype_test.cpp
    executor_utype_test.cpp
    executor_btype_test.cpp
    load_file_test.cpp
    logger_test.cpp
    lui_test_program_test.cpp
    memory_test.cpp
    registers_test.cpp
    simulator_test.cpp
    syscall_test.cpp
)

target_link_libraries(unit_tests
    PRIVATE
        riscv-core
        GTest::gtest_main
)

target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_compile_definitions(unit_tests PRIVATE RISCEMU_TESTING)

include(GoogleTest)
gtest_discover_tests(unit_tests)

# Integration tests: cross-compile small C programs and run them through the
# real ELF loader + CPU + Syscall pipeline. Skipped if a RISC-V cross
# compiler is not on PATH (e.g. GitHub Actions CI).
find_program(RISCV_GCC riscv64-unknown-elf-gcc)

if(RISCV_GCC)
    set(INTEGRATION_DIR ${CMAKE_CURRENT_BINARY_DIR}/integration)
    set(INTEGRATION_PROGRAMS exit42 write_test printf_test)

    foreach(prog ${INTEGRATION_PROGRAMS})
        set(ELF_OUT ${INTEGRATION_DIR}/${prog}.elf)
        add_custom_command(
            OUTPUT ${ELF_OUT}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${INTEGRATION_DIR}
            COMMAND ${RISCV_GCC} -march=rv32i -mabi=ilp32 -static -O1
                    -o ${ELF_OUT}
                    ${CMAKE_CURRENT_SOURCE_DIR}/integration/${prog}.c
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/integration/${prog}.c
            COMMENT "Cross-compiling integration program ${prog}"
        )
        list(APPEND INTEGRATION_ELFS ${ELF_OUT})
    endforeach()

    add_custom_target(integration_elfs ALL DEPENDS ${INTEGRATION_ELFS})

    target_sources(unit_tests PRIVATE integration_test.cpp)
    target_compile_definitions(unit_tests
        PRIVATE RISCEMU_INTEGRATION_DIR="${INTEGRATION_DIR}")
    add_dependencies(unit_tests integration_elfs)
endif()
