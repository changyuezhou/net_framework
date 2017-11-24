cmake_minimum_required(VERSION 2.8.0)

project(nsf_pub)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../make)
include (common)

ice_add_library(nsf_pub
  SOURCE_DIRS ./inc ./src
)
