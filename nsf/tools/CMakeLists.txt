cmake_minimum_required(VERSION 2.8.0)

project(nsf_tools)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../make)
include (common)

#ice_add_executable(nsf_pipe_monitor
#  SOURCE_DIRS ./src/pipe_monitor.cc
#  DEPENDS log magic etc net cache_lib nsf_pub nsf_job_lib
#  LINK_LIBS nsf_job_lib nsf_pub log magic etc net cache_lib pthread dl
#)
