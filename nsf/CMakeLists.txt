cmake_minimum_required(VERSION 2.8.0)

project(nsf)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/make/)
include (common)

add_subdirectory(public)
#add_subdirectory(ap)
#add_subdirectory(job)
#add_subdirectory(ha)
#add_subdirectory(ctrl)
#add_subdirectory(tools)
#add_subdirectory(plugins)
