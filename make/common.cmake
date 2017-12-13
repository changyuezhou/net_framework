cmake_minimum_required(VERSION 2.8.0)

option(M64 "Generate 64 bits program" on)
if(M64)
  set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}   -m64 -fPIC -std=c++11")
  set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -m64 -fPIC -std=c++11")
else()
  set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}   -m32 -march=i686 -std=c++11")
  set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -m32 -march=i686 -std=c++11")
endif()

SET(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}   -DNSF_DEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNSF_DEBUG")

set(WARN_FLAGS "-Wall -O0 -Wextra -Wformat=2 -Wno-unused-parameter -Wno-missing-field-initializers -Wmissing-include-dirs -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wshadow -Woverloaded-virtual")
set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}   -pipe ${WARN_FLAGS}")
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -pipe ${WARN_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
