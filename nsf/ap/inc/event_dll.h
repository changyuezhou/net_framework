// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_AP_INC_EVENT_DLL_H_
#define NSF_AP_INC_EVENT_DLL_H_

#include <stdlib.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/magic/inc/dll.h"

namespace nsf {
  namespace ap {
    using std::string;
    using lib::magic::DLL;

    typedef INT32 (*_TcpHandleInit)(const CHAR * file);
    typedef INT32 (*_TcpHandleConnected)(INT32 h, const CHAR * ip, INT32 port);
    typedef INT32 (*_TcpHandleDisconnected)(INT32 h, const CHAR * ip, INT32 port);
    typedef INT32 (*_FinishTcpHandle)();
    typedef INT32 (*_MsgHandleInit)(const CHAR * file);
    typedef INT32 (*_MsgHandleUnpack)(const CHAR * data, INT32 size);
    typedef INT32 (*_MsgHandleRoute)(const CHAR * data, INT32 size);
    typedef INT32 (*_MsgHandleProcessing)(const CHAR * data, INT32 size);
    typedef INT32 (*_MsgHandleFinish)(const CHAR * data, INT32 size);

    class EventDLL {
     public:
       EventDLL() {}
       ~EventDLL() {}

     public:
       INT32 LoadingMsgHandler(const string & file);

     public:
       INT32 InitTcpHandler(const string & file);
       INT32 TcpHandlerConnected(INT32 h, const string & ip, INT32 port);
       INT32 TcpHandlerDisconnected(INT32 h, const string & ip, INT32 port);
       INT32 FinishTcpHandle();

       INT32 MsgHandlerUnpack(const CHAR * data, INT32 size);
       INT32 MsgHandlerRoute(const CHAR * data, INT32 size);

     private:
       _TcpHandleInit tcp_handler_init_;
       _TcpHandleConnected tcp_handler_connected_;
       _TcpHandleDisconnected tcp_handler_disconnected_;
       _FinishTcpHandle finish_tcp_handler_;

       _MsgHandleUnpack msg_handler_unpack_;
       _MsgHandleRoute msg_handler_route_;
       DLL dll_;
    };
  }  // namespace ap
}  // namespace nsf

#endif  // NSF_AP_INC_EVENT_DLL_H_
