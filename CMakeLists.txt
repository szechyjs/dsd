project(dsd)
cmake_minimum_required(VERSION 2.6)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${dsd_SOURCE_DIR}/cmake/")

include(git_revision)
git_describe(GIT_TAG)

find_package(LibSndFile REQUIRED)
find_package(LibMbe REQUIRED)
find_package(ITPP REQUIRED)
find_package(RTLSDR REQUIRED)

FILE(GLOB SRCS *.c *.cpp)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_ver.c.in" "${CMAKE_CURRENT_BINARY_DIR}/git_ver.c" @ONLY)
list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/git_ver.c" git_ver.h)

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}"
    "${LIBSNDFILE_INCLUDE_DIR}"
    "${LIBMBE_INCLUDE_DIR}"
    "${ITPP_INCLUDE_DIR}"
    "${RTLSDR_INCLUDE_DIR}"
)

SET(LIBS ${LIBS} ${LIBMBE_LIBRARY} ${LIBSNDFILE_LIBRARY} ${ITPP_LIBRARY} ${RTLSDR_LIBRARY})

ADD_EXECUTABLE(dsd ${SRCS})
TARGET_LINK_LIBRARIES(dsd ${LIBS})

# create a library for use with unit tests
ADD_LIBRARY(libdsd ${SRCS})
TARGET_LINK_LIBRARIES(libdsd ${LIBS})

install(TARGETS dsd DESTINATION bin)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

option(DISABLE_TEST "Disable building of test framework." OFF)

if (NOT DISABLE_TEST)
    add_subdirectory(test)
endif()
