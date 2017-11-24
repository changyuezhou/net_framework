// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_AP_INC_UNIX_DGRAM_HANDLE_H_
#define NSF_AP_INC_UNIX_DGRAM_HANDLE_H_

#include <sys/socket.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/net/inc/sock.h"
#include "nsf/ap/inc/handler.h"

namespace nsf {
  namespace ap {
    using lib::net::UnixDGram;

    class UnixDGramHandler: public Handler {
     public:
       UnixDGramHandler(Reactor * reactor, const ServiceItem & config, UINT32 id, EventDLL * dll):Handler(reactor, config, id, dll) {}
       virtual ~UnixDGramHandler() { Destroy(); }

     private:
       UnixDGramHandler(const UnixDGramHandler & r);
       const UnixDGramHandler & operator = (const UnixDGramHandler & r);

     public:
       virtual VOID SetHandler(SOCKET h) { sock_.SetHandle(h); }
       virtual VOID Dump();

     public:
       virtual INT32 HandleRead();
       virtual INT32 HandleWrite();
       virtual INT32 HandleWrite(const CHAR * data, INT32 size);
       virtual INT32 HandleClosed();
       virtual INT32 HandleException();
       virtual INT32 HandleTimeout();

     public:
       virtual SOCKET_TYPE Type() { return sock_.Type(); }
       virtual SOCKET GetHandler() { return sock_.Handle(); }

     public:
       VOID Destroy() {}

     private:
       UnixDGram sock_;
    };
  }  // namespace ap
}  // namespace nsf

#endif  // NSF_AP_INC_UNIX_DGRAM_HANDLE_H_