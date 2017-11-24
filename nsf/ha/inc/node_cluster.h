// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_HA_INC_NODE_CLUSTER_H_
#define NSF_HA_INC_NODE_CLUSTER_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "commlib/public/inc/type.h"
#include "commlib/net/inc/sock_pool.h"
#include "nsf/ha/inc/node.h"
#include "nsf/ha/inc/hash_dll.h"
#include "commlib/magic/src/rbtree.cc"

namespace nsf {
  namespace ha {
    using std::string;
    using std::map;
    using std::vector;
    using std::find;
    using lib::net::SockStream;

    class NodeCluster {
     public:
       enum ROUTE_METHOD {
         ROUND_ROBIN = 1,
         CONSISTENT_HASH = 2
       };

     public:
       typedef struct _virtual_node {
         LONG key_;
         Node * node_;
       } VirtualNode;

       typedef RBTree<VirtualNode> VirtualNodeCluster;
       typedef vector<string> ServiceList;

     public:
       typedef RBTree<VirtualNode>::RBTreeNode RBTreeNode;

     public:
       explicit NodeCluster(HashDLL * hash_dll, INT32 route_method = 1):route_method_(route_method),
                                                                        hash_dll_(hash_dll), robin_(0), center_addr_("") {}
       virtual ~NodeCluster() { Destroy(); }

     public:
       INT32 Initial(const string & center_addr, INT32 timeout);

     public:
       INT32 Join(const Node & node, INT32 min, INT32 max, INT32 timeout, INT32 busy_threshold);
       INT32 Join(const string & addr, INT32 replicates, INT32 min, INT32 max, INT32 timeout, INT32 busy_threshold);

     public:
       NodeList * GetRSNodeList();
       BOOL IsJoined(const string & addr);

     public:
       VOID CheckNodeStatus(INT32 heart_timeout);
       VOID CheckNodeConnection();
       VOID PullNodeList(const string & lb_addr, INT32 replicates, INT32 min, INT32 max, INT32 timeout, INT32 busy_threshold);
       INT32 Send(const CHAR * id, const CHAR * data, INT32 size);

     protected:
       VOID DelNode(Node * node);
       VOID DelNode(const string & addr);
       VOID AddToRBTree(Node * node);
       VOID DelFromRBTree(Node * node);

     protected:
       VOID EncodeCCRequest(const string & lb_addr, CHAR * out, INT32 * size);
       VOID DecodeCCResponse(const CHAR * in, INT32 size, ServiceList & service_list);
       INT32 ConnectToConfigCenter(const string & addr, INT32 timeout);
       INT32 SendByRoundRobin(const CHAR * data, INT32 size);
       INT32 SendByConsistentHash(const CHAR * id, const CHAR * data, INT32 size);
       INT32 SendHeart(Node * node);

     private:
       VOID Destroy();

     private:
       INT32 route_method_;
       HashDLL * hash_dll_;
       INT32 robin_;
       string center_addr_;
       SockStream sock_;
       NodeList rs_node_list_;
       VirtualNodeCluster vs_node_list_;
       ServiceList service_list_;
    };
  }  // namespace ha
}  // namespace nsf

#endif  // NSF_HA_INC_NODE_CLUSTER_H_
