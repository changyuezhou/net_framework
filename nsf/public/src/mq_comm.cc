// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/public/inc/mq_comm.h"
#include "nsf/public/inc/log.h"
#include "nsf/public/inc/err.h"

namespace nsf {
  namespace pub {
    INT32 MQComm::CreateMQ(UINT32 key) {
      fd_ = ::msgget(key, IPC_CREAT | 0666);
      if (0 > fd_) {
        NSF_PUBLIC_LOG_ERROR("msg queue create failed key:" << key << " error code:" << errno << " fd:" << fd_);
        return Err::kERR_MQ_COMM_CREATE_FAILED;
      }

      return 0;
    }

    INT32 MQComm::Attach(UINT32 key) {
      fd_ = ::msgget(key, 0666);
      if (0 > fd_) {
        return Err::kERR_MQ_COMM_CREATE_FAILED;
      }

      return 0;
    }

    VOID MQComm::Destroy() {
      if (0 <= fd_) {
        ::msgctl(fd_, IPC_RMID, NULL);
      }
    }

    INT32 MQComm::Recv(VOID * msg, INT32 size) {
      if (0 > fd_) {
        return Err::kERR_MQ_COMM_HANDLER_INVALID;
      }

      INT32 result = 0;

      do {
        result = ::msgrcv(fd_, msg, size, 0, IPC_NOWAIT);
      } while (-1 == result && errno == EINTR);

      return result;
    }

    INT32 MQComm::Send(const VOID * msg, INT32 size) {
      if (0 > fd_) {
        return Err::kERR_MQ_COMM_HANDLER_INVALID;
      }

      INT32 result = 0;

      do {
        result = ::msgsnd(fd_, msg, size, IPC_NOWAIT);
      } while (-1 == result && errno == EINTR);

      return result;
    }
  }  // namespace pub
}  // namespace nsf
