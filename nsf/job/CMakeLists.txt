cmake_minimum_required(VERSION 2.8.0)

project(nsf_job)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../make)
include (common)

ice_add_library(nsf_job_lib
  SOURCE_DIRS ./inc ./src
)

ice_add_executable(nsf_job
  SOURCE_DIRS ./inc ./src
  DEPENDS log magic etc net cache_lib nsf_pub
  LINK_LIBS nsf_pub log magic etc net cache_lib pthread dl
)