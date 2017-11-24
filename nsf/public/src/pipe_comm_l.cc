// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "commlib/magic/inc/dir.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"
#include "nsf/public/inc/pipe_comm_l.h"
#include "nsf/public/inc/acce.h"

namespace nsf {
  namespace  pub {
    using lib::magic::Dir;

    INT32 PipeCommL::Create(INT32 key, INT32 size, INT32 id) {
      string lock_file_name = Dir::GetBinDir();
      CHAR file_name[256] = {0};
      ::snprintf(file_name, sizeof(file_name), "/.pipe_comm_%d.lock", id);
      lock_file_name = lock_file_name + file_name;

      if (0 != lock_.CreateFileLock(lock_file_name)) {
        NSF_PUBLIC_LOG_ERROR("create file lock failed name:" << lock_file_name);
        return Err::kERR_PIPE_COMM_FILE_LOCK_CREATE_FAILED;
      }

      lock_file_name_ = lock_file_name;

      return PipeComm::Create(key, size);
    }

    VOID PipeCommL::Destroy() {
      CHAR cmd[1024] = {0};
      ::snprintf(cmd, sizeof(cmd), "rm -rf %s", lock_file_name_.c_str());

      ::system(cmd);
    }

    INT32 PipeCommL::Enqueue(const CHAR * data, INT32 size, INT32 ext, const CHAR * uniq) {
      if (unlikely(IsFull())) {
        return Err::kERR_PIPE_COMM_BUFFER_FULL;
      }

      INT32 lock_result = lock_.LockW();

      if (unlikely(0 != lock_result)) {
        return Err::kERR_PIPE_COMM_FILE_LOCK_FAILED;
      }

      INT32 result = PipeComm::Enqueue(data, size, ext, uniq);

      lock_result = lock_.UnLock();
      if (unlikely(0 != lock_result)) {
        NSF_PUBLIC_LOG_ERROR("pipe comm unlock failed");
        return Err::kERR_PIPE_COMM_FILE_UNLOCK_FAILED;
      }

      return result;
    }

    INT32 PipeCommL::Dequeue(CHAR * data, INT32 * size, INT32 * ext, CHAR * uniq, INT32 uniq_size) {
      if (unlikely(IsEmpty())) {
        return Err::kERR_PIPE_COMM_BUFFER_EMPTY;
      }

      INT32 lock_result = lock_.LockW();

      if (unlikely(0 != lock_result)) {
        return Err::kERR_PIPE_COMM_FILE_LOCK_FAILED;
      }

      INT32 result = PipeComm::Dequeue(data, size, ext, uniq, uniq_size);

      lock_result = lock_.UnLock();
      if (unlikely(0 != lock_result)) {
        NSF_PUBLIC_LOG_ERROR("pipe comm unlock failed");
        return Err::kERR_PIPE_COMM_FILE_UNLOCK_FAILED;
      }

      return result;
    }
  }  // namespace pub
}  // namespace nsf