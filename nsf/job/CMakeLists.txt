cmake_minimum_required(VERSION 2.8.0)

project(nsf_job)

AUX_SOURCE_DIRECTORY(./inc DIR_NSF_JOB_INC)
AUX_SOURCE_DIRECTORY(./src DIR_NSF_JOB_SRC)

ADD_LIBRARY(nsf_job_lib ${DIR_NSF_JOB_INC} ${DIR_NSF_JOB_SRC})

ADD_EXECUTABLE(nsf_job ${DIR_NSF_JOB_INC} ${DIR_NSF_JOB_SRC})

TARGET_LINK_LIBRARIES(nsf_job nsf_pub log magic etc net cache_lib pthread dl)