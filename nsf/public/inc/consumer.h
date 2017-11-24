// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_CONSUMER_H_
#define NSF_PUBLIC_INC_CONSUMER_H_

#include <string>
#include "commlib/public/inc/type.h"
#include "nsf/public/inc/pipe_comm_l.h"

namespace nsf {
  namespace pub {
    class Consumer {
     public:
       Consumer() {}
       ~Consumer() { Destroy(); }

     public:
       INT32 CreatePipe(UINT32 key, INT32 size, BOOL lock, INT32 id);

     public:
       INT32 Consuming(CHAR * data, INT32 * size, INT32 * ext, CHAR * uniq, INT32 uniq_size);

     public:
       VOID Dump();

     public:
       VOID Destroy();

     private:
       PipeComm * pipe_;
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_CONSUMER_H_