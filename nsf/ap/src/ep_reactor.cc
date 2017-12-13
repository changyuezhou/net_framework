// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "commlib/magic/inc/tsc.h"
#include "commlib/magic/inc/dir.h"
#include "commlib/magic/inc/crc.h"
#include "nsf/ap/inc/ep_reactor.h"
#include "nsf/ap/inc/tcp_handler.h"
#include "nsf/ap/inc/udp_handler.h"
#include "nsf/ap/inc/unix_stream_handler.h"
#include "nsf/ap/inc/unix_dgram_handler.h"
#include "nsf/ap/inc/dgram_acceptor_handler.h"
#include "nsf/ap/inc/stream_acceptor_handler.h"
#include "nsf/public/inc/acce.h"

namespace nsf {
  namespace ap {
    using lib::magic::Tsc;
    using lib::magic::Dir;
    using lib::magic::CRC;

    INT32 EPReactor::RegisterHandler(Handler * handler, INT32 event) {
      INT32 flag = EPOLLET;
      if (event & Handler::EVENT_READ) {
        flag |= EPOLLIN;
      }
      if (event & Handler::EVENT_WRITE) {
        flag |= EPOLLOUT;
      }

      INT32 ext = handler->ID();
      NSF_AP_LOG_DEBUG("register handle id:" << ext);

      return ep_.Add(handler->GetHandler(), flag, ext);
    }

    INT32 EPReactor::ModHandler(Handler * handler, INT32 event) {
      INT32 flag = EPOLLET;
      if (event & Handler::EVENT_READ) {
        flag |= EPOLLIN;
      }
      if (event & Handler::EVENT_WRITE) {
        flag |= EPOLLOUT;
      }

      INT32 ext = handler->ID();
      return ep_.Mod(handler->GetHandler(), flag, ext);
    }

    INT32 EPReactor::RemoveHandler(Handler * handler) {
      if (NULL == handler) {
        return Err::kERR_EPOLL_REACTOR_HANDLER_INVALID;
      }

      SOCKET h = handler->GetHandler();

      return ep_.Del(h, 0, 0);
    }

    INT32 EPReactor::HandleEvents(UINT32 loop_wait_milli) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "%s_%d", Dir::GetBinDir().c_str(), 255);
      INT32 mq_key = reinterpret_cast<UINT32>(CRC::CRC32(reinterpret_cast<UCHAR *>(key), ::strlen(key)));

      NSF_AP_LOG_DEBUG("epoll reactor create message queue key:" << (UINT32)mq_key);
      MonitorCli cli;
      if (0 != cli.CreateMQ(mq_key, GetGroupID())) {
        NSF_AP_LOG_ERROR("epoll reactor create monitor client failed");
        return 0;
      }
      if (0 != cli.Report()) {
        NSF_AP_LOG_WARN("epoll reactor report status failed code:" << errno);
      }

      UINT32 last_check = ::time(NULL);
      UINT32 last_report = ::time(NULL);
      while (TRUE) {
        if (unlikely(!GetRunning())) {
          return 0;
        }
        // doing send data
        for (INT32 i = 0; i < consumer_size_; ++i) {
          INT32 size = max_receive_;
          INT32 index = -1;
          CHAR uniq[256] = {0};
          while (0 == consumer_[i]->Consuming(receive_data_, &size, &index, uniq, sizeof(uniq))) {
            if (likely(0 <= index && total_handler_ > index)) {
              if (unlikely(handler_table_[index]->IDLE())) {
                NSF_AP_LOG_WARN("epoll reactor processing response handler closed before write index:" << index << " size:" << size);
                continue;
              }
              if (unlikely(handler_table_[index]->IsUniqChanged(uniq))) {
                NSF_AP_LOG_WARN("epoll reactor processing response handler uniq changed before write index:" << index << " size:" << size);
                continue;
              }
              INT32 result = handler_table_[index]->HandleWrite(receive_data_, size);
              if (unlikely(0 != result)) {
                NSF_AP_LOG_WARN("epoll reactor processing response handle write failed index:" << index << " size:" << size);
              }

              size = max_receive_;
            }
          } // loop for consuming
        }  // loop for consumer object

        EPResult result = ep_.Wait(loop_wait_milli);
        for (INT32 i = 0; i < result.first; ++i) {
          INT32 index = result.second[i].data.u64;
          INT32 event = result.second[i].events;

          NSF_AP_LOG_DEBUG("epoll reactor handler index:" << index << " event:" << event);
          if (likely(event & EPOLLOUT)) {
            if (0 != handler_table_[index]->HandleWrite()) {
              NSF_AP_LOG_WARN("epoll reactor handler " << handler_table_[index]->Type() << " write failed id:" << handler_table_[index]->ID());
            }
          }

          if (likely(event & EPOLLIN)) {
            if (0 != handler_table_[index]->HandleRead()) {
              NSF_AP_LOG_WARN("epoll reactor handler " << handler_table_[index]->Type() << " read failed id:" << handler_table_[index]->ID());
            }
          }

          if (unlikely(event & ~(EPOLLIN|EPOLLOUT))) {
            if (0 != handler_table_[index]->HandleClosed()) {
              NSF_AP_LOG_WARN("epoll reactor handler " << handler_table_[index]->Type() << " close failed id:" << handler_table_[index]->ID());
            }
          }
        }

        UINT32 now = ::time(NULL);
        if (unlikely(1 <= (now - last_report))) {
          last_report = now;
          if (0 != cli.Report()) {
            NSF_AP_LOG_WARN("epoll reactor report status failed code:" << errno);

            break;
          }
        }
        if (unlikely(2 <= (now - last_check))) {
          last_check = now;
          INT32 check_timeout_result = CheckTimeout(now);
          if (unlikely(0 != check_timeout_result)) {
            NSF_AP_LOG_WARN("epoll reactor check timeout failed result:" << check_timeout_result);
          }
        }
      }

      return 0;
    }

    INT32 EPReactor::CheckTimeout(UINT32 now) {
      for (INT32 i = 0; i < total_handler_; ++i) {
        if (handler_table_[i]->IDLE()) {
          continue;
        }
        if (handler_table_[i]->CheckTimeout(now)) {
          handler_table_[i]->HandleTimeout();
        }
      }
      return 0;
    }

    SockStream * EPReactor::StreamAcceptorBuilding(SOCKET_TYPE type, const string & addr, INT32 backlog) {
      SockStream * sock = NULL;
      if (Sock::TCP == type) {
        sock = new SockStream();
      } else if (Sock::UNIX_STREAM == type) {
        sock = new UnixStream();
      }

      NetAddr local;
      local.Serialize(addr);
      INT32 result = 0;
      if (NULL == sock || 0 != (result = sock->Listen(local, backlog))) {
        NSF_AP_LOG_ERROR("stream acceptor listen failed:" << result << " ip:" << local.GetIp() << " port:" << local.GetPort() << " Path:" << local.GetPath());
        return NULL;
      }

      return sock;
    }

    SockDGram * EPReactor::DGramAcceptorBuilding(SOCKET_TYPE type, const string & addr) {
      SockDGram * sock = NULL;
      if (Sock::UDP == type) {
        sock = new SockDGram();
      } else if (Sock::UNIX_DGRAM == type) {
        sock = new UnixDGram();
      }

      NetAddr local;
      local.Serialize(addr);
      if (NULL == sock || 0 != sock->Listen(local)) {
        return NULL;
      }

      return sock;
    }

    INT32 EPReactor::BuildingHandlerPools(SOCKET_TYPE type, Handler ** table_head, INT32 size, const ServiceItem & service) {
      ProducerResult p_result = channel_.GetProducerList();
      if (Sock::TCP == type) {
        for (INT32 i = 1; i <= size; ++i) {
          table_head[i] = new TcpHandler(this, service, i + table_head[0]->ID(), &dll_);
          if (NULL == table_head[i]) {
            return Err::kERR_HANDLER_BUILDING_FAILED;
          }

          table_head[i]->SetProducer(p_result.first, p_result.second);
        }
      } else if (Sock::UDP == type) {
        for (INT32 i = 1; i <= size; ++i) {
          table_head[i] = new UdpHandler(this, service, i + table_head[0]->ID(), &dll_);
          if (NULL == table_head[i]) {
            return Err::kERR_HANDLER_BUILDING_FAILED;
          }
          table_head[i]->SetProducer(p_result.first, p_result.second);
        }
      } else if (Sock::UNIX_STREAM == type) {
        for (INT32 i = 1; i <= size; ++i) {
          table_head[i] = new UnixStreamHandler(this, service, i + table_head[0]->ID(), &dll_);
          if (NULL == table_head[i]) {
            return Err::kERR_HANDLER_BUILDING_FAILED;
          }
          table_head[i]->SetProducer(p_result.first, p_result.second);
        }
      } else if (Sock::UNIX_DGRAM == type) {
        for (INT32 i = 1; i <= size; ++i) {
          table_head[i] = new UnixDGramHandler(this, service, i + table_head[0]->ID(), &dll_);
          if (NULL == table_head[i]) {
            return Err::kERR_HANDLER_BUILDING_FAILED;
          }
          table_head[i]->SetProducer(p_result.first, p_result.second);
        }
      } else {
        return Err::kERR_HANDLER_BUILDING_FAILED;
      }

      return 0;
    }

    Handler * EPReactor::HandlerBuilding(const ServiceItem & service, INT32 id) {
      Handler * handler = NULL;
      Handler ** table_head = handler_table_ + id;
      SOCKET_TYPE sock_type = Sock::TCP;
      if (0 == ::strncasecmp(service.type_.c_str(), "tcp", 3)) {
        sock_type = Sock::TCP;
      } else if (0 == ::strncasecmp(service.type_.c_str(), "udp", 3)) {
        sock_type = Sock::UDP;
      } else if (0 == ::strncasecmp(service.type_.c_str(), "unix_stream", 11)) {
        sock_type = Sock::UNIX_STREAM;
      } else if (0 == ::strncasecmp(service.type_.c_str(), "unix_dgram", 10)) {
        sock_type = Sock::UNIX_DGRAM;
      }

      if (Sock::TCP == sock_type || Sock::UNIX_STREAM == sock_type) {
        SockStream * sock = StreamAcceptorBuilding(sock_type, service.addr_, service.backlog_);
        if (NULL == sock) {
          NSF_AP_LOG_ERROR("building stream sock failed addr:" << service.addr_);
          return NULL;
        }
        handler = new StreamAcceptorHandler(this, service, id, &dll_, sock, table_head + 1, service.max_fd_);
        if (NULL == handler) {
          NSF_AP_LOG_ERROR("building stream acceptor handler failed");
          return NULL;
        }
      }

      if (Sock::UDP == sock_type || Sock::UNIX_DGRAM == sock_type) {
        SockDGram * sock = DGramAcceptorBuilding(sock_type, service.addr_);
        if (NULL == sock) {
          NSF_AP_LOG_ERROR("building dgram sock failed");
          return NULL;
        }
        handler = new DGramAcceptorHandler(this, service, id, &dll_, sock, table_head + 1, service.max_fd_);
        if (NULL == handler) {
          NSF_AP_LOG_ERROR("building dgram acceptor handler failed");
          return NULL;
        }
      }

      handler->SetID(id);
      *table_head = handler;
      if (0 != BuildingHandlerPools(sock_type, table_head, service.max_fd_, service)) {
        return NULL;
      }

      table_head += 1;

      return handler;
    }

    INT32 EPReactor::Create(const ServiceList & service_list, const PipeList & pipe_list, const Plugins & plugins) {
      INT32 result = CreatePipeList(pipe_list);
      if (0 != result) {
        return result;
      }

      result = CreatePlugins(plugins);
      if (0 != result) {
        return result;
      }

      return CreateServiceHandler(service_list);
    }

    INT32 EPReactor::CreatePlugins(const Plugins & plugins) {
      INT32 result = dll_.LoadingMsgHandler(plugins.msg_handler_file_);
      if (0 != result) {
        NSF_AP_LOG_ERROR("epoll reactor loading msg handler failed");
        return result;
      }

      result = dll_.InitTcpHandler(plugins.msg_handler_etc_);
      if (0 != result) {
        NSF_AP_LOG_ERROR("epoll reactor tcp handler initial failed");
        return result;
      }

      NSF_AP_LOG_DEBUG("epoll reactor create plugins successed");

      return 0;
    }

    INT32 EPReactor::CreateServiceHandler(const ServiceList & service_list) {
      for (ServiceList::const_iterator it = service_list.begin(); it != service_list.end(); ++it) {
        total_handler_ += it->max_fd_;
        total_handler_++;
      }

      INT32 result = ep_.Create(total_handler_);
      if (0 != result) {
        NSF_AP_LOG_ERROR("epoll create failed");
        return Err::kERR_EPOLL_CREATE_FAILED;
      }

      handler_table_ = reinterpret_cast<Handler **>(::malloc(total_handler_*(sizeof(Handler *))));
      if (NULL == handler_table_) {
        NSF_AP_LOG_ERROR("epoll reactor create handler table failed");
        return Err::kERR_HANDLER_CREATE_FAILED;
      }

      INT32 id = 0;
      for (ServiceList::const_iterator it = service_list.begin(); it != service_list.end(); ++it) {
        Handler * service_handler = HandlerBuilding(*it, id);
        if (NULL == service_handler) {
          NSF_AP_LOG_ERROR("epoll reactor create " << it->type_ << " handler failed");
          return Err::kERR_HANDLER_CREATE_FAILED;
        }
        id += it->max_fd_ + 1;

        result = RegisterHandler(service_handler, Handler::EVENT_READ);
        if (0 != result) {
          return result;
        }
      }

      return 0;
    }

    INT32 EPReactor::CreatePipeList(const PipeList & pipe_list) {
      INT32 size = pipe_list.size();
      INT32 max_buffer = 0;
      for (INT32 i = 0; i < size; ++i) {
        if (max_buffer < pipe_list[i].size_) {
          max_buffer = pipe_list[i].size_;
        }
      }
      max_buffer = max_buffer;
      receive_data_ = reinterpret_cast<CHAR *>(::malloc(max_buffer));
      if (NULL == receive_data_) {
        return Err::kERR_EPOLL_REACTOR_MALLOC_RECEIVE_BUFFER_FAILED;
      }

      max_receive_ = max_buffer;

      INT32 result = channel_.CreatePipeList(pipe_list);
      if (0 != result) {
        return result;
      }

      ConsumerResult c_result = channel_.GetConsumerList();
      consumer_ = c_result.first;
      consumer_size_ = c_result.second;

      return 0;
    }

    VOID EPReactor::Dump() {
      for (INT32 i = 0; i < total_handler_; ++i) {
        handler_table_[i]->Dump();
      }
    }
  }  // namespace ap
}  // namespace nsf
