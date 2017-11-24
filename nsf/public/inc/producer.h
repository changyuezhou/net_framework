// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_PRODUCER_H_
#define NSF_PUBLIC_INC_PRODUCER_H_

#include <string>
#include "commlib/public/inc/type.h"
#include "nsf/public/inc/pipe_comm_l.h"

namespace nsf {
  namespace pub {
    class Producer {
     public:
       Producer() {}
       ~Producer() { Destroy(); }

     public:
       INT32 CreatePipe(UINT32 key, INT32 size, BOOL lock, INT32 id);

     public:
       VOID Destroy();

     public:
       INT32 Producing(const CHAR * data, INT32 size, INT32 ext, const CHAR * uniq);

     public:
       VOID Dump();

      private:
        PipeComm * pipe_;
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_PRODUCER_H_