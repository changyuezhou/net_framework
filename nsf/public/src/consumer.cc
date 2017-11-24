// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"
#include "nsf/public/inc/consumer.h"
#include "nsf/public/inc/acce.h"

namespace nsf {
  namespace pub {
    INT32 Consumer::CreatePipe(UINT32 key, INT32 size, BOOL lock, INT32 id) {
      NSF_PUBLIC_LOG_DEBUG("consumer key:" << key);
      NSF_PUBLIC_LOG_DEBUG("consumer size:" << size);
      NSF_PUBLIC_LOG_DEBUG("consumer id:" << id);
      NSF_PUBLIC_LOG_DEBUG("consumer lock:" << lock);

      if (TRUE == lock) {
        pipe_ = new PipeCommL();
      } else {
        pipe_ = new PipeComm();
      }

      if (NULL == pipe_) {
        return Err::kERR_CONSUMER_PIPE_INVALID;
      }

      return pipe_->Create(key, size, id);
    }

    VOID Consumer::Destroy() {
      if (NULL != pipe_) {
        delete pipe_;
        pipe_ = NULL;
      }
    }

    INT32 Consumer::Consuming(CHAR * data, INT32 * size, INT32 * ext, CHAR * uniq, INT32 uniq_size) {
      if (unlikely(NULL == pipe_)) {
        return Err::kERR_CONSUMER_PIPE_INVALID;
      }

      return pipe_->Dequeue(data, size, ext, uniq, uniq_size);
    }

    VOID Consumer::Dump() {
      if (NULL == pipe_) {
        return ;
      }

      NSF_PUBLIC_LOG_WARN("consumer *****************************************");
      pipe_->Dump();
    }
  }  // namespace pub
}  // namespace nsf