// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_HA_INC_CONFIG_H_
#define NSF_HA_INC_CONFIG_H_

#include <stdlib.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "nsf/public/inc/pipe_comm.h"

namespace nsf {
  namespace ha {
    using std::string;
    using lib::etc::conf;
    using lib::etc::ini;
    using nsf::pub::PipeConfInfo;

    class Config {
     public:
       Config() {}
       ~Config() {}

     public:
       INT32 Loading(const string & file);
       INT32 Loading(const conf & config);
       INT32 LoadingPipeInfo(const conf & config);
       INT32 LoadingConnectPool(const conf & config);
       INT32 LoadingCenterAndRoute(const conf & config);

     public:
       const PipeConfInfo & GetPipeInfo() { return pipe_info_; }
       INT32 GetConnectPoolMin() { return connect_pool_min_; }
       INT32 GetConnectPoolMax() { return connect_pool_max_; }
       INT32 GetConnectPoolTimeout() { return connect_pool_timeout_; }
       INT32 GetHeartTimeout() { return heart_timeout_; }
       INT32 GetBusyThreshold() { return busy_threshold_; }
       INT32 GetRoute() { return route_; }
       INT32 GetReplicates() { return replicates_; }
       const string & GetCenterAddr() { return center_addr_; }
       const string & GetLBAddr() { return lb_addr_; }
       const string & GetHashFile() { return hash_file_; }

     private:
       PipeConfInfo pipe_info_;
       INT32 connect_pool_min_;
       INT32 connect_pool_max_;
       INT32 connect_pool_timeout_;
       INT32 heart_timeout_;
       INT32 busy_threshold_;
       INT32 route_;
       INT32 replicates_;
       string center_addr_;
       string lb_addr_;
       string hash_file_;
    };
  }  // namespace ha
}  // namespace nsf

#endif  // NSF_HA_INC_CONFIG_H_
