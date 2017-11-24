// Copyright (c) 2016 bruce qi. All rights reserved.

#include <string.h>
#include "nsf/ha/inc/ha.h"
#include "nsf/ha/inc/log.h"

namespace nsf {
  namespace ha {
    INT32 HA::Initial(const string & file) {
      NSF_HA_LOG_DEBUG("ha initial start ............................................");
      config_ = new Config();
      if (NULL == config_) {
        NSF_HA_LOG_ERROR("nsf ha allocate config failed");

        return -1;
      }

      if (config_->Loading(file)) {
        NSF_HA_LOG_ERROR("nsf ha loading config failed");

        return -1;
      }

      NSF_HA_LOG_DEBUG("ha initial config loading success ............................................");

      agent_ = new Agent();
      if (NULL == agent_) {
        NSF_HA_LOG_ERROR("nsf ha allocate agent failed");

        return -1;
      }

      if (agent_->Initial(*config_)) {
        NSF_HA_LOG_ERROR("nsf ha initial agent failed");

        return -1;
      }

      NSF_HA_LOG_DEBUG("ha initial agent initial success ............................................");

      node_cluster_ = new NodeCluster(agent_->GetHashDLL(), config_->GetRoute());
      if (NULL == node_cluster_) {
        NSF_HA_LOG_ERROR("nsf ha allocate node cluster failed");

        return -1;
      }

      if (0 != node_cluster_->Initial(config_->GetCenterAddr(), config_->GetConnectPoolTimeout())) {
        NSF_HA_LOG_ERROR("nsf ha initial node cluster failed");

        return -1;
      }

      NSF_HA_LOG_DEBUG("ha initial node cluster initial success ............................................");

      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "%s_%d", Dir::GetBinDir().c_str(), 255);
      INT32 mq_key = reinterpret_cast<UINT32>(CRC::CRC32(reinterpret_cast<UCHAR *>(key), ::strlen(key)));

      if (0 != cli_.CreateMQ(mq_key, config_->GetPipeInfo().job_id_)) {
        NSF_HA_LOG_ERROR("create message queue failed key:" << (UINT32)mq_key);
        return -1;
      }

      NSF_HA_LOG_DEBUG("ha initial report success mq_key:" << mq_key << " ............................................");

      return 0;
    }

    INT32 HA::DoEvent() {
      if (NULL == config_ || NULL == agent_ || NULL == node_cluster_ || need_stop_) {
        NSF_HA_LOG_ERROR("ha parameter is invalid ................................");

        return -1;
      }

      INT32 size = config_->GetPipeInfo().size_;
      CHAR * in = reinterpret_cast<CHAR *>(::malloc(size));
      CHAR * out = reinterpret_cast<CHAR *>(::malloc(size));
      INT32 in_size = size;
      INT32 out_size = size;

      if (NULL == in || NULL == out) {
        NSF_HA_LOG_ERROR("malloc in and out failed size:" << size);

        return -1;
      }

      if (0 != cli_.Report()) {
        NSF_HA_LOG_ERROR("ha report status failed code:" << errno);
        return -1;
      }
      UINT32 last_report = ::time(NULL);
      UINT32 last_pull = 0;
      UINT32 last_check = 0;
      NSF_HA_LOG_DEBUG("ha do event start ............................................");

      while (TRUE) {
        if (need_stop_) {
          NSF_HA_LOG_INFO("ha is stop ....................................");
          break;
        }

        UINT32 now = ::time(NULL);
        if (1 <= (now - last_report)) {
          last_report = now;
          if (0 != cli_.Report()) {
            NSF_HA_LOG_WARN("ha report status failed code:" << errno << " ...................");

            break;
          }
        }

        NodeList * node_list = node_cluster_->GetRSNodeList();
        if ((60 < (now - last_pull)) || (0 >= node_list->size())) {
          node_cluster_->PullNodeList(config_->GetLBAddr(), config_->GetReplicates(),
                                      config_->GetConnectPoolMin(), config_->GetConnectPoolMax(),
                                      config_->GetConnectPoolTimeout(), config_->GetBusyThreshold());

          last_pull = now;
        }

        if (NULL == node_list || 0 >= node_list->size()) {
          NSF_HA_LOG_WARN("ha node cluster is empty ....................................");

          ::usleep(1000);
          continue;
        }

        in_size = out_size = size;
        CHAR id[512] = {0};
        INT32 id_size = sizeof(id);
        if (0 == agent_->WaitForData(in, &in_size, id, &id_size)) {
          if (0 != node_cluster_->Send(id, in, in_size)) {
            NSF_HA_LOG_WARN("ha send data to node cluster failed " << " size:" << in_size);
          }
        }

        for (NodeList::iterator it = node_list->begin(); it != node_list->end(); ++it) {
          EPResult ep_result = it->second->WaitForData(1);
          for (INT32 i = 0; i < ep_result.first; ++i) {
            EventExt * event_ext = reinterpret_cast<EventExt *>(ep_result.second[i].data.u64);
            INT32 event = ep_result.second[i].events;
            SockPool * sock_pool = event_ext->pool_;
            Sock * sock = event_ext->sock_;
            INT32 index = event_ext->index_;

            if (event & EPOLLIN) {
              in_size = size;
              if (0 >= (in_size = sock_pool->Read(index, in, in_size))) {
                NSF_HA_LOG_WARN("ha sock pool index:" << index << " closed by real service:" << it->first);
              } else {
                if (0 != agent_->SendData(in, in_size)) {
                  NSF_HA_LOG_WARN("ha send data to client failed real service:" << it->first);
                }
              }
            }

            if (event & ~(EPOLLIN|EPOLLOUT)) {
              NSF_HA_LOG_WARN(" index:" << index << " event:" << event << " socket:" << sock->Handle() << " EVENT_IN:" << EPOLLIN << " EVENT_OUT:" << EPOLLOUT << " .................");
              sock_pool->CloseSock(index);
            }
          }
        }

        if (1 <= (now - last_check)) {
          last_check = now;
          node_cluster_->CheckNodeStatus(config_->GetHeartTimeout());
          node_cluster_->CheckNodeConnection();
        }

        ::usleep(1000);
      }

      ::free(in);
      ::free(out);

      return 0;
    }
  }  // namespace ha
}  // namespace nsf