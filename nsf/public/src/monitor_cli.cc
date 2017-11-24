// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <string>
#include "nsf/public/inc/monitor_cli.h"

namespace nsf {
  namespace pub {
    INT32 MonitorCli::CreateMQ(INT32 key, INT32 group_id) {
      group_id_ = group_id;
      return mq_comm_.Attach(key);
    }

    INT32 MonitorCli::Report() {
      INT32 pid = ::getpid();
      
      phb_.gid_ = group_id_;
      phb_.pid_ = pid;
      phb_.timestamp_ = ::time(NULL);

      // NSF_PUBLIC_LOG_DEBUG("monitor client report group id:" << group_id_ << " pid:" << pid);

      return mq_comm_.Send(&phb_, sizeof(phb_));
    }
  }  // namespace pub
}  // namespace nsf