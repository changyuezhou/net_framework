// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_AP_INC_HANDLE_H_
#define NSF_AP_INC_HANDLE_H_

#include <sys/socket.h>
#include <string>
#include <algorithm>
#include "commlib/public/inc/type.h"
#include "commlib/cache/inc/buffer.h"
#include "commlib/net/inc/sock.h"
#include "commlib/net/inc/net_addr.h"
#include "nsf/public/inc/channel.h"
#include "nsf/ap/inc/config.h"
#include "nsf/ap/inc/event_dll.h"

namespace nsf {
  namespace ap {
    using std::string;
    using std::find;
    using lib::cache::Buffer;
    using lib::net::NetAddr;
    using lib::net::Sock;
    using nsf::pub::Channel;
    using nsf::pub::Consumer;
    using nsf::pub::Producer;
    class Reactor;

    class Handler {
     public:
       typedef Config::ServiceList ServiceList;
       typedef Config::ServiceItem ServiceItem;
       typedef Buffer::Result BResult;
       typedef Channel::ConsumerResult ConsumerResult;
       typedef Channel::ProducerResult ProducerResult;

     public:
       typedef Sock::SOCKET_TYPE SOCKET_TYPE;

     public:
       enum EVENT {
         EVENT_READ = 1,
         EVENT_WRITE = 2,
         EVENT_CLOSED = 4,
         EVENT_EXCEPTION = 8,
         EVENT_TIMEOUT = 16,
         EVENT_REGISTER = 32,
         EVENT_UNREGISTER = 64
       };

     public:
       static const INT32 RECV_BUFFER_SIZE = 64*1024;

     public:
       Handler(Reactor * reactor, const ServiceItem & config, UINT32 id, EventDLL * dll):id_(id), timestamp_(0),
               reactor_(reactor), producer_(NULL), producer_size_(0), route_robin_(0), dll_(dll), config_(config) {}
       explicit Handler(UINT32 id):id_(id) {}
       virtual ~Handler() { Destroy(); }

     private:
       Handler(const Handler & r);
       const Handler & operator=(const Handler & r);

     public:
       virtual SOCKET_TYPE Type() = 0;
       virtual VOID SetHandler(SOCKET h) = 0;
       virtual SOCKET GetHandler() = 0;

       virtual VOID SetHandlerRcvBuf(INT32 size) {};
       virtual VOID SetHandlerSendBuf(INT32 size) {};

     public:
       INT32 ID() const { return id_; }
       VOID SetID(INT32 id) { id_ = id; }
       VOID SetProducer(Producer ** producer, INT32 size) {
         producer_ = producer;
         producer_size_ = size;
       }
       BOOL IDLE() { return (0 == timestamp_); }
       BOOL IsUniqChanged(const string & uniq) {
         return (0 != ::strncasecmp(uniq.c_str(), remote_.Deserialize().c_str(), uniq.length()));
       }
       BOOL CheckTimeout(UINT32 now) {
         if (config_.timeout_ <= (now - timestamp_)) {
           return TRUE;
         }

         return FALSE;
       }
       VOID SetActiveTime(UINT32 timestamp) { timestamp_ = timestamp; }
       VOID SetLocal(const NetAddr & local) { local_ = local; }
       VOID SetRemote(const NetAddr & remote) { remote_ = remote; }

       VOID Clear() {
         r_.Reset();
         w_.Reset();
         timestamp_ = 0;
       }

     public:
       virtual INT32 HandleRead() = 0;
       virtual INT32 HandleWrite() = 0;
       virtual INT32 HandleWrite(const CHAR * data, INT32 size) = 0;
       virtual INT32 HandleClosed() = 0;
       virtual INT32 HandleException() = 0;
       virtual INT32 HandleTimeout() = 0;

     public:
       INT32 HandleConnecting(INT32 h, const string & ip, INT32 port);
       INT32 HandleLargerPackage();
       INT32 HandleUnpack(const CHAR * data, INT32 size);
       INT32 HandleRoute(const CHAR * data, INT32 size);
       INT32 HandleBack(const CHAR * data, INT32 size);
       INT32 HandleClosing(INT32 h, const string & ip, INT32 port);
       BOOL Filter(const string & ip);

     public:
       INT32 DoReceive(const CHAR * data, INT32 size, INT32 ext, INT32 route);

     public:
       virtual VOID Dump() = 0;

     public:
       VOID Destroy() {
         r_.Free();
         w_.Free();
       }

     protected:
       UINT32 id_;
       UINT32 timestamp_;
       Buffer r_;
       Buffer w_;
       NetAddr local_;
       NetAddr remote_;
       Reactor * reactor_;
       Producer ** producer_;
       INT32 producer_size_;
       INT32 route_robin_;
       EventDLL * dll_;
       ServiceItem config_;
       static CHAR receive_buffer_[RECV_BUFFER_SIZE];
       static INT32 receive_size_;
    };
  }  // namespace ap
}  // namespace lib

#endif  // NSF_AP_INC_HANDLE_H_
