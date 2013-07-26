project(dsd)
cmake_minimum_required(VERSION 2.6)
 
FILE(GLOB SRCS *.c)
 
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}" "${CMAKE_INSTALL_PREFIX}/include")
LINK_DIRECTORIES("${CMAKE_INSTALL_PREFIX}/lib")
 
ADD_EXECUTABLE(dsd ${SRCS})
TARGET_LINK_LIBRARIES(dsd mbe)
 
install(TARGETS dsd DESTINATION bin)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)