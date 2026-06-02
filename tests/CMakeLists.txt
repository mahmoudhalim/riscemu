add_executable(unit_tests
    decoder_test.cpp
    executor_itype_test.cpp
    executor_rtype_test.cpp
    executor_utype_test.cpp
    load_file_test.cpp
    lui_test_program_test.cpp
    memory_test.cpp
    registers_test.cpp
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

include(GoogleTest)
gtest_discover_tests(unit_tests)
