// Copyright (c) 2016 bruce qi. All rights reserved.

#include <string.h>
#include "commlib/magic/inc/timeFormat.h"
#include "nsf/ha/inc/node.h"
#include "nsf/ha/inc/log.h"

namespace nsf {
  namespace ha {
    using lib::magic::TimeFormat;

    INT32 Node::Connect(INT32 min, INT32 max, INT32 timeout, INT32 busy_threshold) {
      sock_pool_ = new SockPool(TRUE, addr_, min, max, timeout, busy_threshold);
      if (NULL == sock_pool_) {
        NSF_HA_LOG_ERROR("nsf ha node allocate object failed addr:" << addr_);
        return -1;
      }

      if (0 != sock_pool_->Initial()) {
        NSF_HA_LOG_ERROR("nsf ha node sock pool initial failed");

        return -1;
      }

      return 0;
    }

    INT32 Node::Connect(const string & addr, INT32 min, INT32 max, INT32 timeout, INT32 busy_threshold) {
      addr_ = addr;
      return Connect(min, max, timeout, busy_threshold);
    }

    INT32 Node::Send(const CHAR * data, INT32 size) {
      if (sock_pool_) {
        return sock_pool_->Write(data, size);
      }

      return 0;
    }

    EPResult Node::WaitForData(INT32 millisecond) {
      if (NULL == sock_pool_) {
        return make_pair(0, reinterpret_cast<epoll_event *>(NULL));
      }

      return sock_pool_->WaitForData(millisecond);
    }

    BOOL Node::CheckIsTimeout(INT32 timeout) {
      if (NULL == sock_pool_) {
        return TRUE;
      }

      INT64 now = TimeFormat::GetCurTimestampLong();
      INT64 pool_last_active = sock_pool_->GetPoolLastActive();
      if ((0 < pool_last_active) && ((now - pool_last_active) >= (timeout * 1000))) {
        NSF_HA_LOG_WARN("node addr:" << addr_ << " is timeout:" << timeout << " now:" << now << " pool_last_active:" << pool_last_active << " .........");
        delete sock_pool_;

        return TRUE;
      }

      return FALSE;
    }

    VOID Node::CheckConnection() {
      if (sock_pool_) {
        sock_pool_->ValidConnect();
      }
    }

    VOID Node::Destroy() {
      if (sock_pool_) {
        delete sock_pool_;
      }
    }
  }  // namespace ha
}  // namespace nsf

