project(dsd)
cmake_minimum_required(VERSION 3.5)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

include(git_revision)
git_describe(GIT_TAG)

option(DSD_USE_BUNDLED_DEPS "Use contrib dependencies" ON)

if(DSD_USE_BUNDLED_DEPS)

    # ---------- mbelib ----------
    set(DISABLE_TEST ON CACHE BOOL "" FORCE)
    add_subdirectory(contrib/mbelib)
    set(MBE_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/contrib/mbelib)
    set(MBE_LIBRARY mbe-static)

    # ---------- libsndfile ----------
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(ENABLE_CPACK OFF CACHE BOOL "" FORCE)
    add_subdirectory(contrib/libsndfile)

    # ---------- IT++ ----------
    set(OLD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(contrib/itpp)

    # ---------- PortAudio ----------
    set(PA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(PA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(PA_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(contrib/portaudio)

else()

    find_package(LibSndFile REQUIRED)
    find_package(MBE REQUIRED)
    find_package(ITPP REQUIRED)
    find_package(LibPortAudio)

endif()

include_directories(SYSTEM ${LIBSNDFILE_INCLUDE_DIR} ${MBE_INCLUDE_DIR} ${ITPP_INCLUDE_DIR})
set(LIBS
    mbe-static
    sndfile
    itpp
)

if(PORTAUDIO_FOUND)
    include_directories(SYSTEM ${PORTAUDIO_INCLUDE_DIRS})
    list(APPEND LIBS ${PORTAUDIO_LIBRARIES})
    add_definitions(-DUSE_PORTAUDIO)
endif(PORTAUDIO_FOUND)

FILE(GLOB SRCS src/*.c src/*.cpp)
FILE(GLOB HEADERS include/*.h include/*.hpp)

configure_file("src/git_ver.c.in" "${CMAKE_CURRENT_BINARY_DIR}/git_ver.c" @ONLY)
list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/git_ver.c")

include_directories("${PROJECT_SOURCE_DIR}/include")

ADD_EXECUTABLE(dsd ${SRCS} ${HEADERS})

target_include_directories(dsd PRIVATE
    ${PROJECT_SOURCE_DIR}/contrib/itpp
    ${CMAKE_BINARY_DIR}
)

# Suppress register keyword warnings in C++17 (for C++ files only)
set_source_files_properties(
    src/p25p1_check_hdu.cpp
    src/p25p1_check_ldu.cpp
    src/p25p1_check_nid.cpp
    src/Hamming.cpp
    PROPERTIES COMPILE_FLAGS "-Wno-register"
)

# Fix for MinGW: Allow multiple definitions of gettimeofday from different libraries
if(MINGW)
    target_link_options(dsd PRIVATE "LINKER:--allow-multiple-definition")
endif()

TARGET_LINK_LIBRARIES(dsd ${LIBS})

include(GNUInstallDirs)
install(TARGETS dsd DESTINATION ${CMAKE_INSTALL_BINDIR})

# man page
find_program(HELP2MAN_FOUND help2man)
if (HELP2MAN_FOUND)
    add_custom_command(TARGET dsd POST_BUILD
        COMMAND help2man
        ARGS -n "Digital Speech Decoder"
            --version-string=${GIT_TAG}
            -o ${CMAKE_CURRENT_BINARY_DIR}/dsd.1
            --no-info
            $<TARGET_FILE:dsd> 
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsd.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif()

# uninstall target
if(NOT TARGET uninstall)

    configure_file(
        "cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND}
                -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

endif()

option(DISABLE_TEST "Disable building of test framework." OFF)

if (NOT DISABLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
