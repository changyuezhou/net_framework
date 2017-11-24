cmake_minimum_required(VERSION 2.8.0)

project(nsf_ctrl)

AUX_SOURCE_DIRECTORY(./inc DIR_NSF_CTRL_INC)
AUX_SOURCE_DIRECTORY(./src DIR_NSF_CTRL_SRC)

ADD_EXECUTABLE(nsf_ctrl ${DIR_NSF_CTRL_INC} ${DIR_NSF_CTRL_SRC})

TARGET_LINK_LIBRARIES(nsf_ctrl nsf_pub log magic etc net cache_lib pthread dl)
