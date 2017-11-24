// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/ap/inc/udp_handler.h"
#include "nsf/public/inc/acce.h"
#include "nsf/ap/inc/reactor.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using nsf::pub::Err;

    INT32 UdpHandler::HandleRead() {
      timestamp_ = ::time(NULL);

      while (TRUE) {
        INT32 result = sock_.ReadFrom(receive_buffer_, RECV_BUFFER_SIZE, &remote_);
        if (unlikely(!Filter(remote_.GetIp()))) {
          return 0;
        }
        if (likely(0 < result && RECV_BUFFER_SIZE > result)) {
          return HandleUnpack(receive_buffer_, result);;
        }

        if (unlikely(0 >= result)) {
          return HandleClosed();
        }

        if (unlikely(RECV_BUFFER_SIZE == result)) {
          result = r_.Append(receive_buffer_, result);
          if (0 != result) {
            NSF_AP_LOG_ERROR("udp socket handler append data failed result:" << result);

            return Err::kERR_UDP_HANDLER_READ_FAILED;
          }
        }
      }

      INT32 result = HandleLargerPackage();
      if (0 != result) {
        NSF_AP_LOG_ERROR("udp socket handler unpacking larger data failed result:" << result);
      }

      return 0;
    }

    INT32 UdpHandler::HandleWrite() {
      Buffer::Result w = w_.GetData();
      INT32 result = 0;
      if (likely(0 < w.first)) {
        result = sock_.WriteTo(w.second, w.first, &remote_);
        if (0 == result) {
          w_.Reset();
        } else {
          NSF_AP_LOG_ERROR("udp socket handler write data failed size:" << w.second);
          return HandleClosed();
        }
      }

      timestamp_ = ::time(NULL);

      return 0;
    }

    INT32 UdpHandler::HandleWrite(const CHAR * data, INT32 size) {
      INT32 result = sock_.WriteTo(data, size, &remote_);
      if (0 == result) {
        NSF_AP_LOG_ERROR("udp socket handler write data failed size:" << size);
        return HandleClosed();
      }

      timestamp_ = ::time(NULL);

      return 0;
    }

    INT32 UdpHandler::HandleClosed() {
      Clear();

      return 0;
    }

    INT32 UdpHandler::HandleException() {
      return HandleClosed();
    }

    INT32 UdpHandler::HandleTimeout() {
      return HandleClosed();
    }

    VOID UdpHandler::Dump() {
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
      NSF_AP_LOG_DEBUG("type :" << "udp_handler");
      NSF_AP_LOG_DEBUG("**************************************************");
    }
  }  // namespace ap
}  // namespace nsf