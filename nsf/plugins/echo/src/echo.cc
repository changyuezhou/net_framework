// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <string.h>
#include "nsf/plugins/echo/inc/echo.h"

namespace nsf {
  namespace plugins {
    INT32 TcpHandleInit(const CHAR * file) {
      return 0;
    }

    INT32 TcpHandlerConnected(INT32 h, const CHAR * ip, INT32 port) {
      return 0;
    }

    INT32 TcpHandlerDisconnected(INT32 h, const CHAR * ip, INT32 port) {
      return 0;
    }

    INT32 FinishTcpHandle() {
      return 0;
    }

    INT32 MsgHandleUnpack(const CHAR * data, INT32 size) {
      return size;
    }

    INT32 MsgHandleRoute(const CHAR * data, INT32 size) {
      return 0;
    }

    INT32 MsgHandleInit(const CHAR * file) {
      return 0;
    }

    INT32 MsgHandleProcessing(const CHAR * data, INT32 size, CHAR * out, INT32 * out_size,
                              AsyncCallBackWrite func, VOID * params, INT32 params_size) {
      const CHAR * resp = "HTTP/1.1 200 OK\r\nServer: spray-can/1.3.2\r\nDate: Thu, 28 May 2015 02:28:30 GMT\r\nContent-Type: text/plain; charset=UTF-8\r\nConnection: keep-alive\r\nContent-Length: 2\n\nOK\n";
      INT32 resp_size = ::strlen(resp);

      if (0 != func(params, resp, resp_size)) {

      }

      *out_size = 0;
      ::memcpy(out, resp, resp_size);
      *out_size = resp_size;

      return 0;
    }

    INT32 MsgHandleFinish() {
      return 0;
    }
  }  // namespace plugins
}  // namespace nsf
