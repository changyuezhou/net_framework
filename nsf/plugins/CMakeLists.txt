cmake_minimum_required(VERSION 2.8.0)

project(nsf)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}../../make)
include (common)

add_subdirectory(hash)
add_subdirectory(echo)
add_subdirectory(emulate)
