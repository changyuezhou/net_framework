// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_AP_INC_EP_REACTOR_H_
#define NSF_AP_INC_EP_REACTOR_H_

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string>
#include <map>
#include "commlib/public/inc/type.h"
#include "commlib/net/inc/sock.h"
#include "commlib/net/inc/net_addr.h"
#include "commlib/net/inc/ep.h"
#include "nsf/ap/inc/stream_acceptor_handler.h"
#include "nsf/ap/inc/reactor.h"
#include "nsf/public/inc/channel.h"
#include "nsf/public/inc/monitor_cli.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace ap {
    using std::pair;
    using nsf::pub::Err;
    using lib::net::EP;
    using lib::net::Sock;
    using lib::net::SockStream;
    using lib::net::UnixStream;
    using lib::net::SockDGram;
    using lib::net::UnixDGram;
    using lib::net::NetAddr;
    using nsf::pub::Consumer;
    using nsf::pub::MonitorCli;

    class EPReactor: public Reactor {
     public:
       typedef Handler::EVENT EVENT;
       typedef Sock::SOCKET_TYPE SOCKET_TYPE;
       typedef pair<INT32, epoll_event *> EPResult;
       typedef Channel::ConsumerResult ConsumerResult;
       typedef Channel::ProducerResult ProducerResult;

     public:
       EPReactor():handler_table_(NULL), total_handler_(0), consumer_(NULL), consumer_size_(0), receive_data_(NULL), max_receive_(0) {}
       virtual ~EPReactor() { Destroy(); }

     public:
       virtual INT32 RegisterHandler(Handler * handler, INT32 event);
       virtual INT32 ModHandler(Handler * handler, INT32 event);
       virtual INT32 RemoveHandler(Handler * handler);
       virtual INT32 HandleEvents(UINT32 loop_wait_milli);
       virtual INT32 Create(const ServiceList & service_list, const PipeList & pipe_list, const Plugins & plugins);
       virtual VOID Dump();

     private:
       Handler * HandlerBuilding(const ServiceItem & service, INT32 id);
       SockStream * StreamAcceptorBuilding(SOCKET_TYPE type, const string & addr, INT32 backlog = 100);
       SockDGram * DGramAcceptorBuilding(SOCKET_TYPE type, const string & addr);
       INT32 BuildingHandlerPools(SOCKET_TYPE type, Handler ** table_head, INT32 size, const ServiceItem & service);

     private:
       INT32 CreateServiceHandler(const ServiceList & service_list);
       INT32 CreatePipeList(const PipeList & pipe_list);
       INT32 CreatePlugins(const Plugins & plugins);

     private:
       INT32 CheckTimeout(UINT32 now);
       INT32 ProcessingResponse();

     public:
       VOID Destroy() {
         if (0 != dll_.FinishTcpHandle()) {
           NSF_AP_LOG_ERROR("plugins finish failed");
         }
         if (NULL != handler_table_) {
           for (INT32 i = 0; i < total_handler_; ++i) {
             delete handler_table_[i];
           }

           ::free(handler_table_);

           handler_table_ = NULL;
         }

         if (receive_data_) {
           ::free(receive_data_);
         }
       }

     private:
       const EPReactor & operator=(const EPReactor &);
       EPReactor(const EPReactor &);

     private:
       Handler ** handler_table_;
       INT32 total_handler_;
       EP ep_;
       Consumer ** consumer_;
       INT32 consumer_size_;
       Channel channel_;
       EventDLL dll_;
       CHAR * receive_data_;
       INT32 max_receive_;
    };
  }  // namespace ap
}  // namespace nsf

#endif  // NSF_AP_INC_EP_REACTOR_H_
