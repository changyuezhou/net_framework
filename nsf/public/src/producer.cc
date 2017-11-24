// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"
#include "nsf/public/inc/producer.h"
#include "nsf/public/inc/acce.h"

namespace nsf {
  namespace pub {
    INT32 Producer::CreatePipe(UINT32 key, INT32 size, BOOL lock, INT32 id) {
      NSF_PUBLIC_LOG_DEBUG("producer key:" << key);
      NSF_PUBLIC_LOG_DEBUG("producer size:" << size);
      NSF_PUBLIC_LOG_DEBUG("producer id:" << id);
      NSF_PUBLIC_LOG_DEBUG("producer lock:" << lock);

      if (TRUE == lock) {
        pipe_ = new PipeCommL();
      } else {
        pipe_ = new PipeComm();
      }

      if (NULL == pipe_) {
        return Err::kERR_PRODUCER_PIPE_INVALID;
      }

      return pipe_->Create(key, size, id);
    }

    VOID Producer::Destroy() {
      if (NULL != pipe_) {
        delete pipe_;

        pipe_ = NULL;
      }
    }

    INT32 Producer::Producing(const CHAR * data, INT32 size, INT32 ext, const CHAR * uniq) {
      if (unlikely(NULL == pipe_)) {
        return Err::kERR_PRODUCER_PIPE_INVALID;
      }

      NSF_PUBLIC_LOG_DEBUG("producer producing size:" << size << " ext:" << ext);

      return pipe_->Enqueue(data, size, ext, uniq);
    }

    VOID Producer::Dump() {
      if (NULL == pipe_) {
        return ;
      }

      NSF_PUBLIC_LOG_WARN("producer *****************************************");
      pipe_->Dump();
    }
  }  // namespace pub
}  // namespace nsf