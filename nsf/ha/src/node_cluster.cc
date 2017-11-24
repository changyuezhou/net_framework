// Copyright (c) 2016 bruce qi. All rights reserved.

#include <string.h>
#include "commlib/net/inc/sock.h"
#include "commlib/magic/inc/str.h"
#include "nsf/ha/inc/node_cluster.h"
#include "nsf/ha/inc/ha.pb.h"
#include "nsf/ha/inc/log.h"

namespace nsf {
  namespace ha {
    using nsf::ha::pb::Request;
    using nsf::ha::pb::Response;
    using lib::net::Sock;
    using lib::net::SockStream;
    using lib::magic::String;

    INT32 NodeCluster::Initial(const string & center_addr, INT32 timeout) {
      center_addr_ = center_addr;
      if (0 != ConnectToConfigCenter(center_addr_, timeout)) {
        NSF_HA_LOG_ERROR("node cluster connect to config center failed addr:" << center_addr_);

        return -1;
      }

      if (0 != vs_node_list_.InitialTree()) {
        NSF_HA_LOG_ERROR("node cluster initial red black tree failed");

        return -1;
      }

      return 0;
    }

    INT32 NodeCluster::Join(const Node & node, INT32 min, INT32 max, INT32 timeout, INT32 busy_threshold) {
      INT32 replicates = node.Replicates();
      const string addr = node.Addr();

      if (IsJoined(addr)) {
        return 0;
      }

      NSF_HA_LOG_DEBUG("node cluster joining addr:" << addr << " replicates:" << replicates << " .......................");

      Node * rs_node = new Node();
      if (NULL == rs_node) {
        return -1;
      }
      rs_node->SetAddr(addr);
      rs_node->SetReplicates(replicates);
      if (0 != rs_node->Connect(min, max, timeout, busy_threshold)) {
        NSF_HA_LOG_ERROR("node cluster connect failed addr:" << addr << " ...................................");

        delete rs_node;

        return -1;
      }

      NSF_HA_LOG_DEBUG("node cluster joining addr:" << addr << " replicates:" << replicates << " connected ");

      if (CONSISTENT_HASH == route_method_) {
        AddToRBTree(rs_node);
      } else {
        service_list_.push_back(addr);
      }

      rs_node_list_.insert(pair<string, Node *>(addr, rs_node));

      NSF_HA_LOG_DEBUG("node cluster joining addr:" << addr << " replicates:" << replicates << " insert into real service list ");

      return 0;
    }

    INT32 NodeCluster::Join(const string & addr, INT32 replicates, INT32 min, INT32 max, INT32 timeout, INT32 busy_threshold) {
      if (rs_node_list_.find(addr) != rs_node_list_.end()) {
        return 0;
      }

      Node node;
      node.SetAddr(addr);
      node.SetReplicates(replicates);

      return Join(node, min, max, timeout, busy_threshold);
    }

    VOID NodeCluster::AddToRBTree(Node * node) {
      if (NULL == node) {
        NSF_HA_LOG_WARN("node cluster joining node is empty .............................");
        return;
      }

      INT32 replicates = node->Replicates();
      const string addr = node->Addr();

      NSF_HA_LOG_DEBUG("node cluster consistent hash joining addr:" << addr << " replicates:" << replicates << " ..................");
      for (INT32 i = 0; i < replicates; ++i) {
        CHAR id[1024] = {0};
        ::snprintf(id, sizeof(id), "%s_%d", addr.c_str(), i);

        LONG key = hash_dll_->Hash(id);

        const string str_key = String::ToString(key);
        RBTreeNode *rb_node = vs_node_list_.Search(str_key);
        if (NULL == rb_node) {
          VirtualNode *vs_node = new VirtualNode();
          if (NULL == vs_node) {
            continue;
          }

          vs_node->node_ = node;
          vs_node->key_ = key;

          NSF_HA_LOG_DEBUG("node cluster joining addr:" << addr << " replicates:" << replicates << " insert into rb tree ");
          vs_node_list_.Insert(str_key, vs_node);
        }
      }
    }

    VOID NodeCluster::DelFromRBTree(Node * node) {
      if (NULL == node) {
        NSF_HA_LOG_WARN("node cluster delete from rb_tree node is empty .............................");
        return;
      }

      INT32 replicates = node->Replicates();
      const string addr = node->Addr();
      NSF_HA_LOG_DEBUG("node cluster consistent hash del node addr:" << addr << " replicates:" << replicates << " ................");
      for (INT32 i = 0; i < replicates; ++i) {
        CHAR id[1024] = {0};
        ::snprintf(id, sizeof(id), "%s_%d", addr.c_str(), i);

        LONG key = hash_dll_->Hash(id);
        const string str_key = String::ToString(key);
        RBTreeNode *rb_node = vs_node_list_.Search(str_key);
        if (NULL == rb_node) {
          continue;
        }

        if (rb_node->data_->key_ != key) {
          continue;
        }

        delete rb_node->data_;
        vs_node_list_.Delete(str_key);
      }
    }

    VOID NodeCluster::DelNode(Node * node) {
      if (NULL == node) {
        return;
      }

      INT32 replicates = node->Replicates();
      const string addr = node->Addr();

      NSF_HA_LOG_DEBUG("node cluster del node addr:" << addr << " replicates:" << replicates << " .......................");

      if (!IsJoined(addr)) {
        return;
      }

      NSF_HA_LOG_DEBUG("node cluster del node addr:" << addr << " replicates:" << replicates << " node is in cluster .....");

      if (CONSISTENT_HASH == route_method_) {
        DelFromRBTree(node);
      } else {
        ServiceList::iterator it = find(service_list_.begin(), service_list_.end(), addr);
        if (it != service_list_.end()) {
          service_list_.erase(it);
        }
      }

      NSF_HA_LOG_DEBUG("node cluster del node addr:" << addr << " replicates:" << replicates << " delete in node list ...");

      rs_node_list_.erase(addr);
      delete node;
    }

    VOID NodeCluster::DelNode(const string & addr) {
      if (0 >= rs_node_list_.size()) {
        return;
      }

      NodeList::iterator it = rs_node_list_.find(addr);
      if (it == rs_node_list_.end()) {
        return;
      }

      DelNode(it->second);
    }

    NodeList * NodeCluster::GetRSNodeList() {
      return &rs_node_list_;
    }

    VOID NodeCluster::CheckNodeStatus(INT32 heart_timeout) {
      if (0 >= rs_node_list_.size()) {
        return;
      }

      NodeList::iterator it = rs_node_list_.begin();
      vector<string> need_del;
      while (it != rs_node_list_.end()) {
        if (it->second->CheckIsTimeout(heart_timeout)) {
          need_del.push_back(it->first);
        } else {
          if (0 != SendHeart(it->second)) {
            NSF_HA_LOG_WARN("node cluster service:" << it->first << " send heart failed ............................");
          }
        }
        ++it;
      }

      if (need_del.size()) {
        for (vector<string>::iterator vit = need_del.begin(); vit != need_del.end(); ++vit) {
          NSF_HA_LOG_WARN("node cluster service:" << *vit << " is timeout ...........................................");
          rs_node_list_.erase(*vit);
          DelNode(*vit);
        }
      }
    }

    VOID NodeCluster::CheckNodeConnection() {
      if (0 >= rs_node_list_.size()) {
        return;
      }
      NodeList::iterator it = rs_node_list_.begin();
      while (it != rs_node_list_.end()) {
        it->second->CheckConnection();
        ++it;
      }
    }

    INT32 NodeCluster::SendHeart(Node * node) {
      if (NULL == node) {
        return -1;
      }

      CHAR data[1024*2] = {0};
      INT32 data_size = sizeof(data);
      if (hash_dll_) {
        hash_dll_->EncodeHeat(data, &data_size);
      }
      if (0 >= data_size || sizeof(data) == data_size) {
        NSF_HA_LOG_ERROR("node cluster encode heart failed ............................................");

        return -1;
      }

      return node->Send(data, data_size);
    }

    INT32 NodeCluster::Send(const CHAR * id, const CHAR * data, INT32 size) {
      NSF_HA_LOG_DEBUG("node cluster send data ........................");

      if (ROUND_ROBIN == route_method_) {
        return SendByRoundRobin(data, size);
      } else if (CONSISTENT_HASH == route_method_) {
        return SendByConsistentHash(id, data, size);
      }

      NSF_HA_LOG_WARN("node cluster send data don't support this method .................");

      return 0;
    }

    INT32 NodeCluster::SendByRoundRobin(const CHAR * data, INT32 size) {
      NSF_HA_LOG_DEBUG("node cluster send data by round robin ........................");

      INT32 node_size = service_list_.size();
      for (INT32 i = 0; i < node_size; ++i) {
        robin_ = robin_%node_size;

        NodeList::iterator it = rs_node_list_.find(service_list_[robin_]);
        if (it != rs_node_list_.end()) {
          NSF_HA_LOG_DEBUG("node cluster send data  robin:" << robin_ << " service:" << it->first);

          if (NULL != it->second) {
            robin_++;
            return it->second->Send(data, size);
          }
        }

        robin_++;
      }

      return -1;
    }

    INT32 NodeCluster::SendByConsistentHash(const CHAR * id, const CHAR * data, INT32 size) {
      NSF_HA_LOG_DEBUG("node cluster send data by consistent hash ........................");

      INT32 node_size = rs_node_list_.size();
      if (0 >= node_size) {

        NSF_HA_LOG_ERROR("node cluster send by consistent hash has no node ...................");

        return -1;
      }

      if (NULL == id || 0 >= String::Trim(id).length()) {
        NSF_HA_LOG_ERROR("node cluster consistent hash id is invalid ........................");

        return 0;
      }
      LONG key = hash_dll_->Hash(id);

      if (0 >= vs_node_list_.TreeSize()) {
        NSF_HA_LOG_ERROR("node cluster has no node");

        return -1;
      }

      RBTreeNode *rb_node = vs_node_list_.upper_bound_first(String::ToString(key));
      if (NULL == rb_node) {
        NSF_HA_LOG_ERROR("node cluster node is invalid key:" << key << " ...................................");
        return -1;
      }

      if (NULL == rb_node->data_ || rb_node->data_->node_) {
        NSF_HA_LOG_ERROR("node cluster node is invalid key:" << key);

        return -1;
      }

      NSF_HA_LOG_DEBUG("node cluster consistent hash send data key:" << key << " service:" << rb_node->data_->node_->Addr());

      return rb_node->data_->node_->Send(data, size);
    }

    BOOL NodeCluster::IsJoined(const string & addr) {
      if (0 >= rs_node_list_.size()) {
        return FALSE;
      }

      if (rs_node_list_.find(addr) == rs_node_list_.end()) {
        return FALSE;
      }

      return TRUE;
    }

    VOID NodeCluster::PullNodeList(const string & lb_addr, INT32 replicates, INT32 min, INT32 max, INT32 timeout, INT32 busy_threshold) {
      CHAR out[1024*4] = {0};
      INT32 out_size = sizeof(out);
      EncodeCCRequest(lb_addr, out, &out_size);
      if (0 >= out_size) {
        NSF_HA_LOG_ERROR("node cluster encode config center request failed .....................");
        return;
      }

      if (!sock_.IsValid() || (0 >= sock_.Write(out, out_size))) {
        sock_.Close();
        if (0 != ConnectToConfigCenter(center_addr_, timeout)) {
          NSF_HA_LOG_ERROR("node cluster connect to center failed addr:" << center_addr_);
        }

        NSF_HA_LOG_WARN("node cluster connection with config center is disconnect.....................");

        return;
      }

      CHAR in[1024*4] = {0};
      INT32 in_size = sizeof(in);
      if (0 >= (in_size = sock_.Read(in, in_size, timeout))) {
        NSF_HA_LOG_WARN("node cluster config center not response result:" << in_size << " timeout:" << timeout << " ..........");

        return;
      }

      ServiceList service_list;
      DecodeCCResponse(in, in_size, service_list);

      INT32 service_size = service_list.size();
      NSF_HA_LOG_DEBUG("node cluster read node list from config center success size:" << service_size << "....................");

      for (INT32 i = 0; i < service_size; ++i) {
        Join(service_list[i], replicates, min, max, timeout, busy_threshold);
      }

      if (0 >= rs_node_list_.size()) {
        NSF_HA_LOG_WARN("node cluster config center delete all nodes or has no node alive service size:" << service_size << " ..........");

        return;
      }

      NodeList::iterator it = rs_node_list_.begin();
      while (it != rs_node_list_.end()) {
        if (0 >= service_size || (find(service_list.begin(), service_list.end(), it->first) == service_list.end())) {
          NSF_HA_LOG_DEBUG("node cluster load balance address:" << lb_addr << " service address:" << it->first << " is deleted by config center .....");
          DelNode(it->second);
        }

        ++it;
      }
    }

    INT32 NodeCluster::ConnectToConfigCenter(const string & addr, INT32 timeout) {
      INT32 result = 0;
      if (0 != (result = sock_.Create())) {
        NSF_HA_LOG_ERROR("node cluster addr:" << addr << " create socket failed");
        return result;
      }

      if (0 != (result = sock_.Connect(addr, timeout))) {
        NSF_HA_LOG_ERROR("node cluster addr:" << addr << " connect service failed");
        return result;
      }

      return 0;
    }

    VOID NodeCluster::EncodeCCRequest(const string & lb_addr, CHAR * out, INT32 * size) {
      Request req;
      req.set_addr(lb_addr);
      INT32 byte_size = req.ByteSize();
      if (byte_size >= *size) {
        NSF_HA_LOG_ERROR("node cluster encode config center request failed");
        *size = 0;
        return;
      }
      req.SerializeToArray(out, byte_size);
      *size = byte_size;
    }

    VOID NodeCluster::DecodeCCResponse(const CHAR * in, INT32 size, ServiceList & service_list) {
      Response resp;
      if (FALSE == resp.ParseFromArray(in, size)) {
        NSF_HA_LOG_WARN("node cluster config center response is invalid");

        return ;
      }

      INT32 node_size = resp.addr_size();
      for (INT32 i = 0; i < node_size; ++i) {
        NSF_HA_LOG_DEBUG("node cluster load balance service address:" << resp.addr(i) << " ........");

        service_list.push_back(resp.addr(i));
      }
    }

    VOID NodeCluster::Destroy() {
      vs_node_list_.FreeTree();
      if (rs_node_list_.size()) {
        NodeList::iterator it = rs_node_list_.begin();
        while (it != rs_node_list_.end()) {
          delete it->second;
          ++it;
        }
      }

      rs_node_list_.clear();

      if (sock_.IsValid()) {
        sock_.Close();
      }
    }
  }  // namespace ha
}  // namespace nsf

