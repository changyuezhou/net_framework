// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_HA_INC_HA_H_
#define NSF_HA_INC_HA_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include "commlib/public/inc/type.h"
#include "commlib/net/inc/ep.h"
#include "commlib/net/inc/sock_pool.h"
#include "nsf/ha/inc/config.h"
#include "nsf/ha/inc/agent.h"
#include "nsf/ha/inc/node.h"
#include "nsf/ha/inc/node_cluster.h"
#include "nsf/public/inc/monitor_cli.h"
#include "commlib/magic/inc/dir.h"
#include "commlib/magic/inc/crc.h"

namespace nsf {
  namespace ha {
    using std::string;
    using std::map;
    using std::pair;
    using lib::net::Sock;
    using lib::net::SockPool;
    using lib::net::EP;
    using lib::magic::Dir;
    using lib::magic::CRC;
    using nsf::pub::MonitorCli;

    typedef EP::EPResult EPResult;
    typedef SockPool::EventExt EventExt;

    class HA {
     public:
       HA(): agent_(NULL), node_cluster_(NULL), config_(NULL), need_stop_(FALSE) {}
       virtual ~HA() { Destroy(); }

     public:
       INT32 Initial(const string & file);
       INT32 DoEvent();

     public:
       VOID Stop() { need_stop_ = TRUE; }

     public:
       VOID Destroy() {
         if (agent_) {
           delete agent_;
         }

         if (node_cluster_) {
           delete node_cluster_;
         }

         if (config_) {
           delete config_;
         }
       }

     private:
       Agent * agent_;
       NodeCluster * node_cluster_;
       Config * config_;
       BOOL need_stop_;
       MonitorCli cli_;
    };

  }  // namespace ha
}  // namespace nsf

#endif  // NSF_HA_INC_HA_H_
