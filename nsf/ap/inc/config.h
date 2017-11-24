// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_AP_INC_CONFIG_H_
#define NSF_AP_INC_CONFIG_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "nsf/public/inc/pipe_comm.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using std::string;
    using std::vector;
    using lib::etc::conf;
    using nsf::pub::Err;
    using nsf::pub::PipeConfInfo;
    using std::sort;

    class Config {
     public:
       typedef vector<string> IPList;

     public:
       enum FILTER_REG {
         WHITE = 1,
         BLACK = 2
       };

     public:
       struct ServiceItem {
         string type_;
         string addr_;
         INT32 max_fd_;
         INT32 backlog_;
         INT32 tos_;
         UINT32 timeout_;
         INT32 recv_size_;
         INT32 send_size_;
         BOOL keep_alive_;
         FILTER_REG filter_reg_;
         IPList ip_list_;

         ServiceItem () {
           type_ = "tcp";
           addr_ = "127.0.0.1:10000";
           max_fd_ = 1000000;
           backlog_ = 100;
           tos_ = 0;
           timeout_ = -1;
           recv_size_ = 10240;
           send_size_ = 10240;
           keep_alive_ = TRUE;
           filter_reg_ = BLACK;
           ip_list_.clear();
         };

         ServiceItem (const ServiceItem & r):
           type_(r.type_),
           addr_(r.addr_),
           max_fd_(r.max_fd_),
           backlog_(r.backlog_),
           tos_(r.tos_),
           timeout_(r.timeout_),
           recv_size_(r.recv_size_),
           send_size_(r.send_size_),
           keep_alive_(r.keep_alive_),
           filter_reg_(r.filter_reg_),
           ip_list_(r.ip_list_) {}
       };

       struct Plugins {
         string msg_handler_file_;
         string msg_handler_etc_;
       };

     public:
       typedef vector<ServiceItem> ServiceList;
       typedef vector<PipeConfInfo> PipeList;

     public:
       Config() {}
       ~Config() { Destroy(); }

     public:
       INT32 Loading(const conf & config);

     private:
       INT32 LoadingService(const conf & config);
       INT32 LoadingPipe(const conf & config);
       INT32 LoadingParameters(const conf & config);
       INT32 LoadingPlugins(const conf & config);

     public:
       VOID Destroy() {
         service_list_.clear();
         pipe_list_.clear();
       }

     public:
       const ServiceList & GetServiceList();
       const PipeList & GetPipeList();
       const Plugins & GetPlugins();
       UINT32 GetLoopWait() { return wait_loop_milliseconds_; }
       INT32 GetGroupID() { return group_id_; }

     private:
       UINT32 wait_loop_milliseconds_;
       ServiceList service_list_;
       PipeList pipe_list_;
       Plugins plugins_;
       INT32 group_id_;
    };
  }  // namespace ap
}  // namespace nsf

#endif  // NSF_AP_INC_CONFIG_H_