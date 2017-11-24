// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_PIPE_COMM_L_H_
#define NSF_PUBLIC_INC_PIPE_COMM_L_H_

#include <string>
#include "commlib/public/inc/type.h"
#include "nsf/public/inc/pipe_comm.h"
#include "commlib/magic/inc/fileLock.h"

namespace nsf {
  namespace pub {
    using std::string;
    using lib::magic::FileLock;

    class PipeCommL: public PipeComm {
     public:
       PipeCommL():lock_file_name_("") {}
       virtual ~PipeCommL() { Destroy(); }

     public:
       virtual INT32 Create(INT32 key, INT32 size = 4*1024*1024, INT32 id = 0);

     public:
       virtual INT32 Enqueue(const CHAR * data, INT32 size, INT32 ext, const CHAR * uniq);
       virtual INT32 Dequeue(CHAR * data, INT32 * size, INT32 * ext, CHAR * uniq, INT32 uniq_size);

     public:
       VOID Destroy();

     private:
       FileLock lock_;
       string lock_file_name_;
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_PIPE_COMM_L_H_