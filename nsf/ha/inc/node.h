// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_HA_INC_NODE_H_
#define NSF_HA_INC_NODE_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "commlib/public/inc/type.h"
#include "commlib/net/inc/sock_pool.h"
#include "commlib/net/inc/ep.h"
#include "commlib/magic/inc/rbtree.h"

namespace nsf {
  namespace ha {
    using std::string;
    using std::list;
    using std::map;
    using std::vector;
    using std::pair;
    using std::make_pair;
    using lib::net::EP;
    using lib::net::Sock;
    using lib::net::SockPool;
    using lib::net::SockStream;
    using lib::magic::RBTree;

    typedef EP::EPResult EPResult;

    class Node {
     public:
       Node():addr_(""), replicates_(0), sock_pool_(NULL) {}
       explicit Node(const string & addr):addr_(addr), replicates_(0), sock_pool_(NULL) {}
       virtual ~Node() { Destroy(); }

     public:
       INT32 Connect(INT32 min = 1, INT32 max = 1, INT32 timeout = 10, INT32 busy_threshold = 50);
       INT32 Connect(const string & addr, INT32 min = 1, INT32 max = 1, INT32 timeout = 10, INT32 busy_threshold = 50);
       INT32 Send(const CHAR * data, INT32 size);
       EPResult WaitForData(INT32 millisecond);
       BOOL CheckIsTimeout(INT32 timeout);
       VOID CheckConnection();

     public:
       const string & Addr() const { return addr_; }
       INT32 Replicates() const { return replicates_; }

     public:
       VOID SetAddr(const string & addr) { addr_ = addr; }
       VOID SetReplicates(INT32 replicates) { replicates_ = replicates; }

     private:
       VOID Destroy();

     private:
       string addr_;
       INT32 replicates_;
       SockPool * sock_pool_;
    };

    typedef map<string, Node *> NodeList;
  }  // namespace ha
}  // namespace nsf

#endif  // NSF_HA_INC_NODE_H_
