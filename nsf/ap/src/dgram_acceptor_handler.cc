// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/ap/inc/dgram_acceptor_handler.h"
#include "nsf/public/inc/acce.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using nsf::pub::Err;

    INT32 DGramAcceptorHandler::HandleRead() {
      INT32 idle = GetIdleHandler();
      if (Err::kERR_DGRAM_ACCEPTOR_HANDLER_POOLS_EMPTY == idle) {
        NSF_AP_LOG_ERROR("dgram acceptor handler has no idle handler");
        return Err::kERR_DGRAM_ACCEPTOR_HANDLER_POOLS_EMPTY;
      }

      INT32 result = pools_[idle]->HandleRead();
      if (0 != result) {
        return result;
      }

      pools_[idle]->SetHandler(sock_->Handle());

      return 0;
    }

    INT32 DGramAcceptorHandler::HandleWrite() {
      return 0;
    }

    INT32 DGramAcceptorHandler::HandleClosed() {
      Clear();

      INT32 result = sock_->Close();
      if (0 != result) {
        NSF_AP_LOG_ERROR("dgram acceptor socket handler closed failed");
      }

      NSF_AP_LOG_DEBUG("socket {" << remote_.Deserialize() << "-" << local_.Deserialize() << "} closed");

      return 0;
    }

    INT32 DGramAcceptorHandler::HandleException() {
      return HandleClosed();
    }

    INT32 DGramAcceptorHandler::HandleTimeout() {
      return 0;
    }

    INT32 DGramAcceptorHandler::GetIdleHandler() {
      if (NULL == pools_ || 0 == pools_size_) {
        return Err::kERR_DGRAM_ACCEPTOR_HANDLER_POOLS_EMPTY;
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

    VOID DGramAcceptorHandler::Dump() {
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
      NSF_AP_LOG_DEBUG("type :" << "dgram_acceptor_handler");
      NSF_AP_LOG_DEBUG("**************************************************");
    }
  }  // namespace ap
}  // namespace nsf