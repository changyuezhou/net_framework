// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/public/inc/log.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/acce.h"
#include "nsf/ap/inc/event_dll.h"

namespace nsf {
  namespace ap {
    INT32 EventDLL::LoadingMsgHandler(const string & file) {
      INT32 result = dll_.CreateDll(file);
      if (0 != result) {
        return result;
      }

      tcp_handler_init_ = reinterpret_cast<_TcpHandleInit>(dll_.GetFunctionHandle("TcpHandleInit"));
      tcp_handler_connected_ = reinterpret_cast<_TcpHandleConnected>(dll_.GetFunctionHandle("TcpHandleConnected"));
      tcp_handler_disconnected_ = reinterpret_cast<_TcpHandleDisconnected>(dll_.GetFunctionHandle("TcpHandleDisconnected"));
      finish_tcp_handler_ = reinterpret_cast<_FinishTcpHandle>(dll_.GetFunctionHandle("FinishTcpHandle"));

      msg_handler_unpack_ = reinterpret_cast<_MsgHandleUnpack>(dll_.GetFunctionHandle("MsgHandleUnpack"));
      if (NULL == msg_handler_unpack_) {
        NSF_AP_LOG_WARN("event dll loading unpack handler failed");
      }

      msg_handler_route_ = reinterpret_cast<_MsgHandleRoute>(dll_.GetFunctionHandle("MsgHandleRoute"));
      if (NULL == msg_handler_route_) {
        NSF_AP_LOG_WARN("event dll loading route handler failed");
      }

      return 0;
    }

    INT32 EventDLL::InitTcpHandler(const string & file) {
      if (NULL != tcp_handler_init_) {
        return tcp_handler_init_(file.c_str());
      }

      NSF_AP_LOG_DEBUG("event dll initial tcp handler success");

      return 0;
    }

    INT32 EventDLL::TcpHandlerConnected(INT32 h, const string & ip, INT32 port) {
      if (NULL != tcp_handler_connected_) {
        return tcp_handler_connected_(h, ip.c_str(), port);
      }

      NSF_AP_LOG_DEBUG("event dll tcp handler connected default");

      return 0;
    }

    INT32 EventDLL::TcpHandlerDisconnected(INT32 h, const string & ip, INT32 port) {
      if (NULL != tcp_handler_disconnected_) {
        return tcp_handler_disconnected_(h, ip.c_str(), port);
      }

      NSF_AP_LOG_DEBUG("event dll tcp handler disconnected default");

      return 0;
    }

    INT32 EventDLL::FinishTcpHandle() {
      if (NULL != finish_tcp_handler_) {
        return finish_tcp_handler_();
      }

      NSF_AP_LOG_DEBUG("event dll tcp handler finished default");

      return 0;
    }

    INT32 EventDLL::MsgHandlerUnpack(const CHAR * data, INT32 size) {
      if (likely(NULL != msg_handler_unpack_)) {
        return msg_handler_unpack_(data, size);
      }

      NSF_AP_LOG_DEBUG("event dll msg handler unpack default");

      return size;
    }

    INT32 EventDLL::MsgHandlerRoute(const CHAR * data, INT32 size) {
      if (likely(NULL != msg_handler_route_)) {
        return msg_handler_route_(data, size);
      }

      NSF_AP_LOG_DEBUG("event dll msg handler routing default");

      return 0;
    }
  }  // namespace ap
}  // namespace nsf
