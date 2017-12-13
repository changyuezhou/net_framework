// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/public/inc/mq_comm.h"
#include "nsf/public/inc/proc_report.h"
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

    INT32 MQComm::Recv(VOID * data, INT32 size, INT32 * type) {
      if (0 > fd_) {
        return Err::kERR_MQ_COMM_HANDLER_INVALID;
      }

      if (size < ProcReport::MSG_MAX_LENGTH) {
        return Err::kERR_CTRL_MESSAGE_QUEUE_RECV_BUFFER_LENGTH_TOO_SMALL;
      }

      INT32 result = 0;
      ProcReport::MSG msg;

      do {
        result = ::msgrcv(fd_, &msg, ProcReport::MSG_MAX_LENGTH, 0, IPC_NOWAIT);
      } while (-1 == result && errno == EINTR);

      if (0 < result) {
        *type = msg.msg_type_;
        ::memcpy(data, msg.msg_text_, ProcReport::MSG_MAX_LENGTH);
      }

      return result;
    }

    INT32 MQComm::Send(const VOID * data, INT32 size, INT32 type) {
      if (0 > fd_) {
        return Err::kERR_MQ_COMM_HANDLER_INVALID;
      }

      INT32 result = 0;
      ProcReport::MSG msg;
      msg.msg_type_ = type;
      ::memcpy(msg.msg_text_, data, size);

      do {
        result = ::msgsnd(fd_, &msg, ProcReport::MSG_MAX_LENGTH, IPC_NOWAIT);
      } while (-1 == result && errno == EINTR);

      return result;
    }
  }  // namespace pub
}  // namespace nsf
