project(dsd)
cmake_minimum_required(VERSION 2.6)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${dsd_SOURCE_DIR}/cmake/")

find_package(LibSndFile REQUIRED)
find_package(LibMbe REQUIRED)

FILE(GLOB SRCS *.c)

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}"
    "${LIBSNDFILE_INCLUDE_DIR}"
    "${LIBMBE_INCLUDE_DIR}"
)

SET(LIBS ${LIBS} ${LIBMBE_LIBRARY} ${LIBSNDFILE_LIBRARY})

ADD_EXECUTABLE(dsd ${SRCS})
TARGET_LINK_LIBRARIES(dsd ${LIBS})

install(TARGETS dsd DESTINATION bin)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
