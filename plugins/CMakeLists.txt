cmake_minimum_required(VERSION 2.8.0)

project(plugins)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../make/)
include (common)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)

ADD_SUBDIRECTORY(echo)
