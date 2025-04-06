find_package(Catch2 3 REQUIRED)

FILE(GLOB TEST_SRCS *.cpp ${dsd_SOURCE_DIR}/src/Hamming.cpp)

add_executable(tests ${TEST_SRCS})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(tests)
