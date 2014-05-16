project(dsd)
cmake_minimum_required(VERSION 2.6)

# Add local cmake directory to search path
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${dsd_SOURCE_DIR}/cmake/")

include(git_revision)
git_describe(GIT_TAG)

# Find required libraries
find_package(LibSndFile REQUIRED)
find_package(LibMbe REQUIRED)
find_package(ITPP REQUIRED)

# Add srouce files
FILE(GLOB SRCS src/*.c src/*.cpp)

# Create git version file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/git_ver.c.in" "${CMAKE_CURRENT_BINARY_DIR}/src/git_ver.c" @ONLY)
list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/src/git_ver.c")

# Add include directories
INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/include"
    "${LIBSNDFILE_INCLUDE_DIR}"
    "${LIBMBE_INCLUDE_DIR}"
    "${ITPP_INCLUDE_DIR}"
)

# Set libraries for linking
SET(LIBS ${LIBS} ${LIBMBE_LIBRARY} ${LIBSNDFILE_LIBRARY} ${ITPP_LIBRARY})

# Create dsd executable
ADD_EXECUTABLE(dsd ${SRCS})
TARGET_LINK_LIBRARIES(dsd ${LIBS})

# Create a library for use with unit tests
ADD_LIBRARY(libdsd ${SRCS})
TARGET_LINK_LIBRARIES(libdsd ${LIBS})

# Create an install target
install(TARGETS dsd DESTINATION bin)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# Test options
option(DISABLE_TEST "Disable building of test framework." OFF)

if (NOT DISABLE_TEST)
    add_subdirectory(test)
endif()
