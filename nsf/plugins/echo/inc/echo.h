// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PLUGINS_ECHO_INC_ECHO_H_
#define NSF_PLUGINS_ECHO_INC_ECHO_H_

#include <stdlib.h>
#include <string>
#include "commlib/public/inc/type.h"

namespace nsf {
  namespace plugins {
  	extern "C" {
      typedef INT32 (*AsyncCallBackWrite)(VOID * params, const CHAR * out, INT32 out_size);

      INT32 TcpHandleInit(const CHAR * file);
      INT32 TcpHandlerConnected(INT32 h, const CHAR * ip, INT32 port);
      INT32 TcpHandlerDisconnected(INT32 h, const CHAR * ip, INT32 port);
      INT32 FinishTcpHandle();

      INT32 MsgHandleUnpack(const CHAR * data, INT32 size);
      INT32 MsgHandleRoute(const CHAR * data, INT32 size);
      INT32 MsgHandleInit(const CHAR * file);
      INT32 MsgHandleProcessing(const CHAR * data, INT32 size, CHAR * out, INT32 * out_size,
                                AsyncCallBackWrite func, VOID * params, INT32 params_size);
      INT32 MsgHandleFinish();
    }
  }  // namespace plugins
}  // namespace nsf

#endif  // NSF_PLUGINS_ECHO_INC_ECHO_H_
