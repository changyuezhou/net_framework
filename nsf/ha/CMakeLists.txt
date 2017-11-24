cmake_minimum_required(VERSION 2.8.0)

project(nsf_ha)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../make)
include (common)

add_subdirectory(proto)

ice_add_library(nsf_ha_lib
        SOURCE_DIRS ./inc ./src
        )

add_subdirectory(test)

ice_add_executable(nsf_ha
        SOURCE_DIRS ./inc ./src
        DEPENDS log ha_proto magic etc net cache_lib nsf_pub ext_lib
        LINK_LIBS nsf_pub ha_proto log magic etc net cache_lib ext_lib pthread dl protobuf
        )