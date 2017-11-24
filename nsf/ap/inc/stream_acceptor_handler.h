// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_AP_INC_STREAM_ACCEPTOR_HANDLE_H_
#define NSF_AP_INC_STREAM_ACCEPTOR_HANDLE_H_

#include <sys/socket.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/net/inc/sock.h"
#include "nsf/ap/inc/handler.h"

namespace nsf {
  namespace ap {
    using lib::net::SockStream;

    class StreamAcceptorHandler: public Handler {
     public:
       typedef Config::FILTER_REG FILTER_REG;

     public:
       StreamAcceptorHandler(Reactor * reactor, const ServiceItem & config, UINT32 id, EventDLL * dll, SockStream * sock,
                            Handler ** pools = NULL, INT32 pools_size = 0):Handler(reactor, config, id, dll),
                            pools_(pools), pools_size_(pools_size), sock_(sock), cur_index_(0) {
                              local_.Serialize(config.addr_);
                            }

       virtual ~StreamAcceptorHandler() { Destroy(); }

     private:
       StreamAcceptorHandler(const StreamAcceptorHandler & r);
       const StreamAcceptorHandler & operator = (const StreamAcceptorHandler & r);

     public:
       virtual VOID SetHandler(SOCKET h) { sock_->SetHandle(h); }
       virtual VOID Dump();

     public:
       virtual INT32 HandleRead();
       virtual INT32 HandleWrite();
       virtual INT32 HandleWrite(const CHAR * data, INT32 size) { return 0; }
       virtual INT32 HandleClosed();
       virtual INT32 HandleException();
       virtual INT32 HandleTimeout();

     private:
       INT32 GetIdleHandler();

     public:
       virtual SOCKET_TYPE Type() { return Sock::STREAM_ACCEPTOR; }
       virtual SOCKET GetHandler() { return sock_->Handle(); }

     public:
       VOID Destroy() {
         if (NULL != sock_) {
           delete sock_;
         }
       }

     private:
       Handler ** pools_;
       INT32 pools_size_;
       SockStream * sock_;
       INT32 cur_index_;
    };
  }  // namespace ap
}  // namespace nsf

#endif  // NSF_AP_INC_STREAM_ACCEPTOR_HANDLE_H_