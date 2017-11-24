// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/ap/inc/stream_acceptor_handler.h"
#include "nsf/public/inc/acce.h"
#include "nsf/ap/inc/reactor.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using nsf::pub::Err;

    INT32 StreamAcceptorHandler::HandleRead() {
      if (unlikely(NULL == sock_)) {
        return Err::kERR_STREAM_ACCEPTOR_HANDLER_EMPTY;
      }

      INT32 cnt = 50;

      NSF_AP_LOG_DEBUG("stream accept handler reading ......");
      while ( 0 <= --cnt ) {
        INT32 h = sock_->Accept(&remote_);
        if (likely(0 < h)) {
          if (unlikely(!Filter(remote_.GetIp()))) {
            NSF_AP_LOG_DEBUG("stream acceptor handler filter failed ip:" << remote_.GetIp());
            ::close(h);
            continue;
          }

          if (unlikely(0 != HandleConnecting(h, remote_.GetIp(), remote_.GetPort()))) {
            NSF_AP_LOG_DEBUG("stream acceptor handler connected callback failed ip:" << remote_.GetIp());
            ::close(h);
            continue;
          }
          INT32 idle = GetIdleHandler();
          if (Err::kERR_STREAM_ACCEPTOR_HANDLER_POOLS_EMPTY == idle) {
            NSF_AP_LOG_ERROR("stream acceptor handler has no idle handler");
            return Err::kERR_STREAM_ACCEPTOR_HANDLER_POOLS_EMPTY;
          }

          Handler * handler = pools_[idle];
          handler->SetHandler(h);
          handler->SetHandlerRcvBuf(config_.recv_size_);
          handler->SetHandlerSendBuf(config_.send_size_);
          handler->SetLocal(local_);
          handler->SetRemote(remote_);
          handler->SetActiveTime(::time(NULL));

          reactor_->RegisterHandler(handler, EVENT_READ);

          NSF_AP_LOG_DEBUG("connection {" << local_.Deserialize() << " - " << remote_.Deserialize() << "} established");

        } else {
          return 0;
        }
      }

      return 0;
    }

    INT32 StreamAcceptorHandler::HandleWrite() {
      return 0;
    }

    INT32 StreamAcceptorHandler::HandleClosed() {
      return 0;
    }

    INT32 StreamAcceptorHandler::HandleException() {
      return 0;
    }

    INT32 StreamAcceptorHandler::HandleTimeout() {
      return 0;
    }

    INT32 StreamAcceptorHandler::GetIdleHandler() {
      if (NULL == pools_ || 0 == pools_size_) {
        return Err::kERR_STREAM_ACCEPTOR_HANDLER_POOLS_EMPTY;
      }

      INT32 i = cur_index_;
      while (TRUE) {
        if (TRUE == pools_[i]->IDLE()) {
          break;
        }
        i = (i + 1)%pools_size_;
        if (i == cur_index_) {
          return Err::kERR_STREAM_ACCEPTOR_HANDLER_POOLS_FULL;
        }
      }

      cur_index_ = i;

      return cur_index_;
    }

    VOID StreamAcceptorHandler::Dump() {
      NSF_AP_LOG_DEBUG("**************************************************");
      NSF_AP_LOG_DEBUG("id:" << id_);
      NSF_AP_LOG_DEBUG("timestamp:" << timestamp_);
      NSF_AP_LOG_DEBUG("w capacity:" << w_.GetCapacity());
      NSF_AP_LOG_DEBUG("w size:" << w_.GetSize());
      NSF_AP_LOG_DEBUG("r capacity:" << r_.GetCapacity());
      NSF_AP_LOG_DEBUG("r size:" << r_.GetSize());
      NSF_AP_LOG_DEBUG("local :" << local_.Deserialize());
      NSF_AP_LOG_DEBUG("remote :" << remote_.Deserialize());
      NSF_AP_LOG_DEBUG("pools size :" << pools_size_);
      NSF_AP_LOG_DEBUG("type :" << "stream_acceptor_handler");
      NSF_AP_LOG_DEBUG("**************************************************");
    }
  }  // namespace ap
}  // namespace nsf