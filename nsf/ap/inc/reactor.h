// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_AP_INC_REACTOR_H_
#define NSF_AP_INC_REACTOR_H_

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "nsf/ap/inc/config.h"
#include "nsf/ap/inc/handler.h"
#include "nsf/public/inc/pipe_comm.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using nsf::pub::Err;
    using nsf::pub::PipeConfInfo;

    class Reactor {
     public:
       typedef Config::ServiceList ServiceList;
       typedef Config::ServiceItem ServiceItem;
       typedef Config::PipeList PipeList;
       typedef Config::Plugins Plugins;

     public:
       typedef Handler::EVENT EVENT;

     public:
       Reactor(): running_(TRUE) {}
       virtual ~Reactor() { Destroy(); }

     public:
       virtual INT32 RegisterHandler(Handler * handler, INT32 event) = 0;
       virtual INT32 ModHandler(Handler * handler, INT32 event) = 0;
       virtual INT32 RemoveHandler(Handler * handler) = 0;
       virtual INT32 HandleEvents(UINT32 loop_wait_milli) = 0;

     public:
       virtual INT32 Create(const ServiceList & service_list, const PipeList & pipe_list, const Plugins & plugins) = 0;
       virtual VOID Dump() = 0;

     public:
       VOID Destroy() {}

     public:
       VOID SetGroupID(INT32 id) { group_id_ = id; }
       VOID SetRunning(BOOL r) { running_ = r; }

     public:
       BOOL GetRunning() { return running_; }
       INT32 GetGroupID() { return group_id_; }

     private:
       const Reactor & operator=(const Reactor &);
       Reactor(const Reactor &);

     private:
       INT32 group_id_;
       BOOL running_;
    };
  }  // namespace ap
}  // namespace nsf

#endif  // NSF_AP_INC_REACTOR_H_
