// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/ap/inc/handler.h"
#include "nsf/ap/inc/reactor.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/acce.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using nsf::pub::Err;
    using nsf::pub::Producer;

    CHAR Handler::receive_buffer_[RECV_BUFFER_SIZE];
    INT32 Handler::receive_size_ = 0;

    INT32 Handler::HandleConnecting(INT32 h, const string & ip, INT32 port) {
      if (likely(NULL != dll_)) {
        return dll_->TcpHandlerConnected(h, ip.c_str(), port);
      }

      return 0;
    }

    INT32 Handler::HandleLargerPackage() {
      BResult r_result = r_.GetData();
      const CHAR * data = r_result.second;
      INT32 rest_size = r_result.first;
      INT32 result = 0;
      while (likely(0 < rest_size)) {
        INT32 used_size = rest_size;
        INT32 route = 0;  // add unpack code
        result = DoReceive(data, used_size, id_, route);
        if (unlikely(0 != result)) {
          break;
        }
        data += used_size;
        rest_size -= used_size;
      }

      r_.Reset();

      return result;
    }

    INT32 Handler::HandleUnpack(const CHAR * data, INT32 size) {
      if (unlikely(NULL == dll_)) {
        INT32 index = route_robin_%producer_size_;
        route_robin_ = (route_robin_ + 1)%producer_size_;
        return producer_[index]->Producing(data, size, id_, remote_.Deserialize().c_str());
      }

      INT32 read_size = 0;
      while (size) {
        INT32 pkg_size = dll_->MsgHandlerUnpack(data + read_size, size);
        if (unlikely(0 >= pkg_size)) {
          NSF_AP_LOG_WARN("msg handler unpack failed result:" << pkg_size);
          return pkg_size;
        }
        INT32 route = HandleRoute(data + read_size, pkg_size);
        INT32 result = producer_[route]->Producing(data + read_size, pkg_size, id_, remote_.Deserialize().c_str());
        if (unlikely(0 != result)) {
          NSF_AP_LOG_WARN("handler producing data failed result:" << result);
          return result;
        }
        read_size += pkg_size;
        size -= pkg_size;
      }

      return 0;
    }

    INT32 Handler::HandleRoute(const CHAR * data, INT32 size) {
      INT32 route = route_robin_%producer_size_;

      if (unlikely(NULL == dll_)) {
        route_robin_ = (route_robin_ + 1)%producer_size_;

        return route;
      }

      INT32 dll_route = dll_->MsgHandlerRoute(data, size);
      if (unlikely(0 >= dll_route)) {
        route_robin_ = (route_robin_ + 1)%producer_size_;

        return route;
      }

      return (dll_route - 1)%producer_size_;
    }

    INT32 Handler::DoReceive(const CHAR * data, INT32 size, INT32 ext, INT32 route) {
      if (unlikely(route >= producer_size_)) {
        return Err::kERR_CHANNEL_PRODUCER_INDEX_INVALID;
      }

      return producer_[route]->Producing(data, size, ext, remote_.Deserialize().c_str());
    }

    INT32 Handler::HandleBack(const CHAR * data, INT32 size) {
      return 0;
    }

    INT32 Handler::HandleClosing(INT32 h, const string & ip, INT32 port) {
      if (likely(NULL != dll_)) {
        return dll_->TcpHandlerDisconnected(h, ip.c_str(), port);
      }

      return 0;
    }

    BOOL Handler::Filter(const string & ip) {
      NSF_AP_LOG_DEBUG("handler filter ip:" << ip << " list size:" << config_.ip_list_.size());
      if (config_.filter_reg_ == Config::WHITE) {
        NSF_AP_LOG_DEBUG("handler white filter ......... ");
        if (0 >= config_.ip_list_.size()) {
          return FALSE;
        }
        if (config_.ip_list_.end() == find(config_.ip_list_.begin(), config_.ip_list_.end(), ip)) {
          return FALSE;
        }
      } else {
        NSF_AP_LOG_DEBUG("handler black filter ......... ");
        if (0 < config_.ip_list_.size()) {
          if (config_.ip_list_.end() != find(config_.ip_list_.begin(), config_.ip_list_.end(), ip)) {
            return FALSE;
          }
        }
      }

      return TRUE;
    }
  }  // namespace ap
}  // namespace nsf