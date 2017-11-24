cmake_minimum_required(VERSION 2.8.0)

project(ha_test)

ice_add_executable(rbtreen_test
        ./src/main.cc
        DEPENDS log magic etc ext_lib
        LINK_LIBS log magic etc ext_lib pthread dl
        )