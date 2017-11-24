// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/ap/inc/unix_dgram_handler.h"
#include "nsf/public/inc/acce.h"
#include "nsf/ap/inc/reactor.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using nsf::pub::Err;

    INT32 UnixDGramHandler::HandleRead() {
      while (TRUE) {
        INT32 result = sock_.ReadFrom(receive_buffer_, RECV_BUFFER_SIZE, &remote_);
        if (likely(0 < result && RECV_BUFFER_SIZE > result)) {
          result = r_.Append(receive_buffer_, result);
          if (0 != result) {
            NSF_AP_LOG_ERROR("udp socket handler append data failed result:" << result);
          }

          break;
        }

        if (unlikely(0 >= result)) {
          HandleClosed();

          break;
        }

        if (unlikely(RECV_BUFFER_SIZE == result)) {
          result = r_.Append(receive_buffer_, result);
          if (0 != result) {
            NSF_AP_LOG_ERROR("udp socket handler append data failed result:" << result);

            break;
          }
        }
      }

      timestamp_ = ::time(NULL);

      return 0;
    }

    INT32 UnixDGramHandler::HandleWrite() {
      Buffer::Result w = w_.GetData();
      INT32 result = 0;
      if (likely(0 < w.first)) {
        result = sock_.WriteTo(w.second, w.first, &remote_);
        if (0 == result) {
          w_.Reset();
        } else {
          NSF_AP_LOG_ERROR("unix dgram socket handler write data failed size:" << w.second);
          return HandleClosed();
        }
      }

      timestamp_ = ::time(NULL);

      return 0;
    }

    INT32 UnixDGramHandler::HandleWrite(const CHAR * data, INT32 size) {
      INT32 result = sock_.WriteTo(data, size, &remote_);
      if (0 == result) {
        NSF_AP_LOG_ERROR("unix dgram socket handler write data failed size:" << size);
        return HandleClosed();
      }

      timestamp_ = ::time(NULL);

      return 0;
    }

    INT32 UnixDGramHandler::HandleClosed() {
      Clear();

      return 0;
    }

    INT32 UnixDGramHandler::HandleException() {
      return HandleClosed();
    }

    INT32 UnixDGramHandler::HandleTimeout() {
      return HandleClosed();
    }

    VOID UnixDGramHandler::Dump() {
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
      NSF_AP_LOG_DEBUG("type :" << "unix_dgram_handler");
      NSF_AP_LOG_DEBUG("**************************************************");
    }
  }  // namespace ap
}  // namespace nsf