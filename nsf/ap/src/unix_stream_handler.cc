// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/ap/inc/unix_stream_handler.h"
#include "nsf/public/inc/acce.h"
#include "nsf/ap/inc/reactor.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using nsf::pub::Err;

   INT32 UnixStreamHandler::HandleRead() {
      while (TRUE) {
        INT32 result = sock_.Read(receive_buffer_, RECV_BUFFER_SIZE);
        if (likely(0 < result && RECV_BUFFER_SIZE > result)) {
          NSF_AP_LOG_DEBUG("unix stream socket handler receive " << result << " data " << receive_buffer_);
          result = r_.Append(receive_buffer_, result);
          if (0 != result) {
            NSF_AP_LOG_ERROR("unix stream socket handler append data failed result:" << result);
          }
          break;
        }

        if (unlikely(0 == result)) {
          HandleClosed();
          break;
        }

        if (unlikely(0 > result)) {
          if (EAGAIN != errno && EINTR != errno) {
            HandleException();
          }

          break;
        }

        if (unlikely(RECV_BUFFER_SIZE == result)) {
          result = r_.Append(receive_buffer_, result);
          if (0 != result) {
            NSF_AP_LOG_ERROR("tcp socket handler append data failed result:" << result);
            break;
          }
        }
      }

      timestamp_ = ::time(NULL);

      return 0;
    }

    INT32 UnixStreamHandler::HandleWrite() {
      Buffer::Result w = w_.GetData();
      INT32 result = 0;
      if (likely(0 < w.first)) {
        result = sock_.Write(w.second, w.first);
        if (0 == result) {
          w_.Reset();
        } else {
          NSF_AP_LOG_ERROR("unix stream socket handler write data failed size:" << w.second);
          return HandleClosed();
        }
      }

      timestamp_ = ::time(NULL);

      return 0;
    }

    INT32 UnixStreamHandler::HandleWrite(const CHAR * data, INT32 size) {
      NSF_AP_LOG_DEBUG("unix stream socket handler write ......");
      INT32 result = sock_.Write(data, size);
      if (0 != result) {
        NSF_AP_LOG_ERROR("unix stream socket handler write data failed size:" << size);
        return HandleClosed();
      }

      if (config_.keep_alive_) {
        timestamp_ = ::time(NULL);
      } else {
        return HandleClosed();
      }

      return 0;
    }

    INT32 UnixStreamHandler::HandleClosed() {
      Clear();
      INT32 result = sock_.Close();
      if (0 != result) {
        NSF_AP_LOG_ERROR("unix stream socket handler closed failed");
      }

      NSF_AP_LOG_DEBUG("socket {" << remote_.Deserialize() << "-" << local_.Deserialize() << "} closed");

      return 0;
    }

    INT32 UnixStreamHandler::HandleException() {
      return HandleClosed();
    }

    INT32 UnixStreamHandler::HandleTimeout() {
      return HandleClosed();
    }

    VOID UnixStreamHandler::Dump() {
      NSF_AP_LOG_DEBUG("**************************************************");
      NSF_AP_LOG_DEBUG("id:" << id_);
      NSF_AP_LOG_DEBUG("timestamp:" << timestamp_);
      NSF_AP_LOG_DEBUG("w capacity:" << w_.GetCapacity());
      NSF_AP_LOG_DEBUG("w size:" << w_.GetSize());
      NSF_AP_LOG_DEBUG("r capacity:" << r_.GetCapacity());
      NSF_AP_LOG_DEBUG("r size:" << r_.GetSize());
      NSF_AP_LOG_DEBUG("local :" << local_.Deserialize());
      NSF_AP_LOG_DEBUG("remote :" << remote_.Deserialize());
      NSF_AP_LOG_DEBUG("sock_ :" << sock_.Handle());
      NSF_AP_LOG_DEBUG("type :" << "unix_stream_handler");
      NSF_AP_LOG_DEBUG("**************************************************");
    }
  }  // namespace ap
}  // namespace nsf
