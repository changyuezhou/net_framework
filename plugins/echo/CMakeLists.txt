cmake_minimum_required(VERSION 2.8.0)

project(echo)

AUX_SOURCE_DIRECTORY(./inc DIR_ECHO_INC)
AUX_SOURCE_DIRECTORY(./src DIR_ECHO_SRC)

ADD_LIBRARY(echo SHARED ${DIR_ECHO_INC} ${DIR_ECHO_SRC})
