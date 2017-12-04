cmake_minimum_required(VERSION 2.8.0)

project(nsf)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/make/)
include (common)

ADD_SUBDIRECTORY(commlib)
ADD_SUBDIRECTORY(nsf)
ADD_SUBDIRECTORY(plugins)