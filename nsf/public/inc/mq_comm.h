// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_MQ_COMM_H_
#define NSF_PUBLIC_INC_MQ_COMM_H_

#include <sys/msg.h>
#include <errno.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace pub {
    class MQComm {
     public:
       MQComm() {}
       ~MQComm() {}

     public:
       INT32 CreateMQ(UINT32 key);
       INT32 Attach(UINT32 key);
       VOID Destroy();

       INT32 Recv(VOID * msg, INT32 size);
       INT32 Send(const VOID * msg, INT32 size);

     private:
       INT32 fd_;
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_MQ_COMM_H_
