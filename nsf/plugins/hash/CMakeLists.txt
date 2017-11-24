cmake_minimum_required(VERSION 2.8.0)

project(nsf_plugins_hash)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../make)
include (common)

ice_add_shared_library(nsf_plugins_hash
        SOURCE_DIRS ./inc ./src
        DEPENDS magic
        LINK_LIBS magic
        )