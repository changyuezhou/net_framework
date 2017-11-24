// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_JOB_INC_CONFIG_H_
#define NSF_JOB_INC_CONFIG_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "nsf/public/inc/pipe_comm.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace job {
    using std::string;
    using std::vector;
    using lib::etc::conf;
    using nsf::pub::Err;
    using nsf::pub::PipeConfInfo;

    class Config {
     public:
       struct Plugins {
         string msg_handler_file_;
         string msg_handler_etc_;
       };

     public:
       Config() {}
       ~Config() { Destroy(); }

     public:
       INT32 Loading(const conf & config);
       INT32 LoadingParameters(const conf & config);
       INT32 LoadingPlugins(const conf & config);

     public:
       const PipeConfInfo & GetPipeInfo();
       const Plugins & GetPlugins();
       INT32 GetLoopWaitTime() { return loop_wait_time_; }
       INT32 GetAsyncMaxRequest() { return async_max_request_; }

     public:
       VOID Destroy() {
       }

     private:
       PipeConfInfo pipe_info_;
       INT32 loop_wait_time_;
       INT32 async_max_request_;
       Plugins plugins_;
    };
  }  // namespace job
}  // namespace nsf

#endif  // NSF_JOB_INC_CONFIG_H_
