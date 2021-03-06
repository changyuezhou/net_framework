// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_AP_INC_UDP_HANDLE_H_
#define NSF_AP_INC_UDP_HANDLE_H_

#include <sys/socket.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/net/inc/sock.h"
#include "nsf/ap/inc/handler.h"

namespace nsf {
  namespace ap {
    using lib::net::SockDGram;

    class UdpHandler: public Handler {
     public:
       UdpHandler(Reactor * reactor, const ServiceItem & config, UINT32 id, EventDLL * dll):Handler(reactor, config, id, dll) {}
       virtual ~UdpHandler() { Destroy(); }

     private:
       UdpHandler(const UdpHandler & r);
       const UdpHandler & operator = (const UdpHandler & r);

     public:
       virtual VOID SetHandler(SOCKET h) { sock_.SetHandle(h); }
       virtual SOCKET GetHandler() { return sock_.Handle(); }
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

     public:
       VOID Destroy() {}

     private:
       SockDGram sock_;
    };
  }  // namespace ap
}  // namespace nsf

#endif  // NSF_AP_INC_UDP_ACCEPTOR_HANDLE_H_