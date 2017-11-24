// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_PIPE_COMM_H_
#define NSF_PUBLIC_INC_PIPE_COMM_H_

#include <sys/shm.h>
#include <sys/mman.h>
#include <errno.h>
#include <string>
#include <cstdio>
#include "commlib/public/inc/type.h"
#include "commlib/magic/inc/atomic.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace pub {
    using std::string;

    struct PipeRealTimeInfo {
      INT32 total_len_;
      volatile INT32 r_;
      volatile INT32 w_;
    };

    struct PipeStatInfo {
      atomic_t total_msg_count_;
      atomic_t processed_msg_count_;
      atomic_t total_time_;
      atomic_t max_pkg_;
      atomic_t min_pkg_;
    };

    struct PipeConfInfo {
      INT32 job_id_;
      INT32 size_;
      UINT32 consumer_key_;
      UINT32 producer_key_;
      BOOL lock_;

      VOID Dump() {
        NSF_PUBLIC_LOG_DEBUG("pipe info job id:" << job_id_);
        NSF_PUBLIC_LOG_DEBUG("pipe info size:" << size_);
        NSF_PUBLIC_LOG_DEBUG("pipe info consumer key:" << consumer_key_);
        NSF_PUBLIC_LOG_DEBUG("pipe info producer key:" << producer_key_);
        NSF_PUBLIC_LOG_DEBUG("pipe info lock:" << lock_);
      }
    };

    class PipeComm {
     public:
       const static INT32 MAGIC_NUM = 0xA55A5AA5;
       const static INT32 TAG_LENGTH = 4;
       const static INT32 TAIL_LENGTH = 4;
       const static INT32 DATA_LENGTH = 4;
       const static INT32 EXT_LENGTH = 4;
       const static INT32 UNIQ_LENGTH = 32;
       const static INT32 HEAD_LENGTH = TAG_LENGTH + DATA_LENGTH + EXT_LENGTH + UNIQ_LENGTH;
       const static INT32 DATA_INFO_LENGTH = HEAD_LENGTH + TAIL_LENGTH;

     public:
       PipeComm():ptr_(NULL), rt_info_(NULL), st_info_(NULL), data_(NULL) {}
       virtual ~PipeComm() { Destroy(); }

     public:
       virtual INT32 Create(INT32 key, INT32 size = 4*1024*1024, INT32 id = 0);

     public:
       virtual INT32 Enqueue(const CHAR * data, INT32 size, INT32 ext, const CHAR * uniq);
       virtual INT32 Dequeue(CHAR * data, INT32 * size, INT32 * ext, CHAR * uniq, INT32 uniq_size);

     public:
       VOID Dump();
       BOOL IsEmpty();
       BOOL IsFull();

     public:
       VOID Clear();
       VOID Destroy() {
         ::shmdt((const VOID *)ptr_);
       }

     private:
       CHAR * ptr_;
       PipeRealTimeInfo * rt_info_;
       PipeStatInfo * st_info_;
       CHAR * data_;
       UINT32 key_;
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_PIPE_COMM_H_