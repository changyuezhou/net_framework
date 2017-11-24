// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "commlib/magic/inc/timeFormat.h"
#include "nsf/ap/inc/tcp_handler.h"
#include "nsf/public/inc/acce.h"
#include "nsf/ap/inc/reactor.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using nsf::pub::Err;
    using lib::magic::TimeFormat;

    INT32 TcpHandler::HandleRead() {
      timestamp_ = ::time(NULL);

      while (TRUE) {
        INT32 result = sock_.Read(receive_buffer_, RECV_BUFFER_SIZE);
        NSF_AP_LOG_DEBUG("tcp socket handler receive " << result << " timestamp:" << TimeFormat::GetCurTimestampLong());

        if (likely(0 < result && RECV_BUFFER_SIZE > result)) {
          return HandleUnpack(receive_buffer_, result);
        }

        if (unlikely(0 == result)) {
          return HandleClosed();
        }

        if (unlikely(0 > result)) {
          if (EAGAIN != errno && EINTR != errno) {
            HandleException();

            return Err::kERR_TCP_HANDLER_READ_FAILED;
          }

          break;
        }

        if (unlikely(RECV_BUFFER_SIZE == result)) {
          result = r_.Append(receive_buffer_, result);
          if (0 != result) {
            NSF_AP_LOG_ERROR("tcp socket handler append data failed result:" << result);

            return Err::kERR_TCP_HANDLER_READ_FAILED;
          }
        }
      }

      INT32 result = HandleLargerPackage();
      if (0 != result) {
        NSF_AP_LOG_ERROR("tcp socket handler unpacking larger data failed result:" << result);
      }

      return 0;
    }

    INT32 TcpHandler::HandleWrite() {
      NSF_AP_LOG_DEBUG("tcp socket handler write event ...... ");
      timestamp_ = ::time(NULL);
      BResult w = w_.GetData();
      INT32 result = 0;
      if (likely(0 < w.first)) {
        INT64 start = TimeFormat::GetCurTimestampLong();
        result = sock_.Write(w.second, w.first);
        INT64 end = TimeFormat::GetCurTimestampLong();
        NSF_AP_LOG_DEBUG("tcp socket handler write cost time: " << (end - start) << " timestamp:" << end << " success ......");
        if (w.first == result) {
          NSF_AP_LOG_DEBUG("tcp socket handler write size: " << result << " success ......");
          w_.Reset();
          if (config_.keep_alive_) {
            result = reactor_->ModHandler(this, EVENT_READ);
            if (0 != result) {
              NSF_AP_LOG_ERROR("tcp socket handler modify handler failed result:" << result);
              return result;
            }
          } else {
            return HandleClosed();
          }
        } else if (0 >= result) {
          NSF_AP_LOG_ERROR("tcp socket handler write data failed size:" << w.second);
          return HandleClosed();
        } else {
          w_.Skip(result);
        }
      }

      return 0;
    }

    INT32 TcpHandler::HandleWrite(const CHAR * data, INT32 size) {
      NSF_AP_LOG_DEBUG("tcp socket handler write size: " << size << " ......");
      timestamp_ = ::time(NULL);
      INT64 start = TimeFormat::GetCurTimestampLong();
      INT32 result = sock_.Write(data, size);
      INT64 end = TimeFormat::GetCurTimestampLong();
      NSF_AP_LOG_DEBUG("tcp socket handler write cost time: " << (end - start) << " timestamp:" << end << " success ......");
      if (likely(result == size)) {
        NSF_AP_LOG_DEBUG("tcp socket handler write size: " << size << " success ......");
        if (!config_.keep_alive_) {
          return HandleClosed();
        }

        return 0;
      }

      if (unlikely(0 >= result)) {
        NSF_AP_LOG_ERROR("tcp socket handler write data failed size:" << size << " result:" << result);
        return HandleClosed();
      }

      result = w_.Append(data + result, size - result);
      if (0 != result) {
        NSF_AP_LOG_ERROR("tcp handler write data to w buffer failed result:" << result);
        return result;
      }

      result = reactor_->ModHandler(this, EVENT_WRITE|EVENT_READ);
      if (0 != result) {
        NSF_AP_LOG_ERROR("tcp socket handler modify handler failed result:" << result);
        return result;
      }

      return 0;
    }

    INT32 TcpHandler::HandleClosed() {
      NSF_AP_LOG_DEBUG("tcp socket handler closed start id:" << id_);
      if (unlikely(0 != HandleClosing(sock_.Handle(), remote_.GetIp(), remote_.GetPort()))) {
        NSF_AP_LOG_WARN("tcp socket handler disconnected callback failed");
      }
      Clear();
      if (unlikely(0 != sock_.Close())) {
        NSF_AP_LOG_ERROR("tcp socket handler closed failed");
      }

      NSF_AP_LOG_DEBUG("socket {" << remote_.Deserialize() << "-" << local_.Deserialize() << "} closed");

      return 0;
    }

    INT32 TcpHandler::HandleException() {
      return HandleClosed();
    }

    INT32 TcpHandler::HandleTimeout() {
      NSF_AP_LOG_DEBUG("tcp socket handler timeout id:" << id_);
      return HandleClosed();
    }

    VOID TcpHandler::Dump() {
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
      NSF_AP_LOG_DEBUG("type :" << "tcp_handler");
      NSF_AP_LOG_DEBUG("**************************************************");
    }
  }  // namespace ap
}  // namespace nsf