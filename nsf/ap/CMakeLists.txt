cmake_minimum_required(VERSION 2.8.0)

project(nsf_ap)

AUX_SOURCE_DIRECTORY(./inc DIR_NSF_AP_INC)
AUX_SOURCE_DIRECTORY(./src DIR_NSF_AP_SRC)

ADD_EXECUTABLE(nsf_ap ${DIR_NSF_AP_INC} ${DIR_NSF_AP_SRC})

TARGET_LINK_LIBRARIES(nsf_ap nsf_pub log magic etc net cache_lib pthread dl)

