// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_MONITOR_CLI_H_
#define NSF_PUBLIC_INC_MONITOR_CLI_H_

#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "nsf/public/inc/mq_comm.h"
#include "nsf/public/inc/proc_report.h"
#include "nsf/public/inc/err.h"

namespace nsf {
  namespace pub {
    using lib::etc::conf;

    class MonitorCli {
     public:
       typedef ProcReport::ProcHeartbeat ProcHeartbeat;

     public:
       MonitorCli() {}
       ~MonitorCli() {}

     public:
       INT32 CreateMQ(INT32 key, INT32 group_id);

     public:
       INT32 Report();

     private:
       MQComm mq_comm_;
       INT32 group_id_;
       ProcHeartbeat phb_;
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_MONITOR_CLI_H_