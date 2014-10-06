project(dsd)
cmake_minimum_required(VERSION 2.6)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${dsd_SOURCE_DIR}/cmake/")

include(git_revision)
git_describe(GIT_TAG)

find_package(LibSndFile REQUIRED)
find_package(LibMbe REQUIRED)
find_package(ITPP REQUIRED)
find_package(LibPortAudio)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
ADD_DEFINITIONS(-DUSE_WINDOWS_AUDIO)
ADD_DEFINITIONS(-DUSE_WINDOWS_SERIAL)
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "SunOS" )
ADD_DEFINITIONS(-DUSE_SOLARIS_AUDIO)
else ()
ADD_DEFINITIONS(-DUSE_BSD_AUDIO)
endif(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")



FILE(GLOB SRCS *.c *.cpp)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_ver.c.in" "${CMAKE_CURRENT_BINARY_DIR}/git_ver.c" @ONLY)
list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/git_ver.c" git_ver.h)

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}"
    "${LIBSNDFILE_INCLUDE_DIR}"
    "${LIBMBE_INCLUDE_DIR}"
    "${ITPP_INCLUDE_DIR}"
)

SET(LIBS ${LIBS} ${LIBMBE_LIBRARY} ${LIBSNDFILE_LIBRARY} ${ITPP_LIBRARY})

if(PORTAUDIO_FOUND)
SET(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} "${PORTAUDIO_INCLUDE_DIRS}")
SET(LIBS ${LIBS} ${PORTAUDIO_LIBRARIES})
add_definitions(-DUSE_PORTAUDIO)
endif(PORTAUDIO_FOUND)

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

option(DISABLE_TEST "Disable building of test framework." OFF)

if (NOT DISABLE_TEST)
    add_subdirectory(test)
endif()
