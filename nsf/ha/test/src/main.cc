// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <unistd.h>
#include <signal.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/magic/inc/daemonize.h"
#include "commlib/magic/inc/singletonHolder.h"
#include "commlib/magic/src/rbtree.cc"
#include "commlib/log/inc/handleManager.h"
#include "nsf/ha/inc/log.h"
#include "nsf/ha/inc/node_cluster.h"
#include "external/lib/inc/md5.h"

using std::string;
using lib::magic::SingletonHolder;
using lib::log::HandleManager;
using nsf::ha::NodeCluster;
using lib::magic::RBTree;
using lib::magic::String;
using ext::lib::MD5;

typedef NodeCluster::VirtualNode VirtualNode;
typedef RBTree<VirtualNode> VirtualNodeRBTree;
typedef RBTree<VirtualNode>::RBTreeNode RBTreeNode;

VOID AddToTree(VirtualNodeRBTree * rb_tree, LONG key) {
  RBTreeNode *rb_node = rb_tree->Search(String::ToString(key));
  if (NULL == rb_node) {
    VirtualNode *vs_node = new VirtualNode();
    if (NULL == vs_node) {
      return;
    }

    vs_node->node_ = NULL;
    vs_node->key_ = key;

    rb_tree->Insert(String::ToString(key), vs_node);
  }
}

VOID DelFromTree(VirtualNodeRBTree * rb_tree, LONG key) {
  RBTreeNode *rb_node = rb_tree->Search(String::ToString(key));
  if (NULL == rb_node) {
    return;
  }

  if (rb_node->data_->key_ != key) {
    return;
  }

  delete rb_node->data_;
  rb_tree->Delete(String::ToString(key));
}

VOID Print(RBTreeNode * node, VOID * data) {
  if (NULL == node) {
    return;
  }

  NSF_HA_LOG_DEBUG("####################################################");
  NSF_HA_LOG_DEBUG("key:" << node->key_);
  NSF_HA_LOG_DEBUG("color:" << node->color_);
  if (NULL != node->parent_) {
    NSF_HA_LOG_DEBUG("parent:" << node->parent_->key_);
  }
  if (NULL != node->left_) {
    NSF_HA_LOG_DEBUG("left:" << node->left_->key_);
  }
  if (NULL != node->right_) {
    NSF_HA_LOG_DEBUG("right:" << node->right_->key_);
  }
  NSF_HA_LOG_DEBUG("####################################################");
}

LONG Hash(const CHAR * id) {
  if (NULL == id || 0 >= String::Trim(id).length()) {
    return 0;
  }

  LONG hash = 0;
  MD5 md5(id);
  const string digest = md5.hex_digest();

  for(INT32 i = 0; i < 4; ++i)
  {
    hash += ((LONG)(digest.c_str()[i*4 + 3]&0xFF) << 24)
            | ((LONG)(digest.c_str()[i*4 + 2]&0xFF) << 16)
            | ((LONG)(digest.c_str()[i*4 + 1]&0xFF) <<  8)
            | ((LONG)(digest.c_str()[i*4 + 0]&0xFF));
  }

  return hash;
}

INT32 main(INT32 argc, CHAR ** argv) {
  if (4 > argc) {
    printf("Usage: ha [config file] [ip] [replicates]\r\n");
    return -1;
  }

  if (0 >= ::atoi(argv[3])) {
    printf("replicates must larger than 0");
    return -1;
  }

  if (0 != HandleManager::GetFrameInstance()->Initial(argv[1])) {
    printf("initial log file:%s failed\r\n", argv[1]);
    delete HandleManager::GetFrameInstance();

    return -1;
  }

  VirtualNodeRBTree rb_tree;
  const string addr = argv[2];
  INT32 replicates = ::atoi(argv[3]);

  if (rb_tree.InitialTree()) {
    NSF_HA_LOG_DEBUG("node cluster initial rb tree failed  ...............................");
    return -1;
  }


  NSF_HA_LOG_DEBUG("node cluster joining addr:" << addr << " replicates:" << replicates << " insert into rb tree ");

  INT32 * key = reinterpret_cast<INT32 *>(::malloc(sizeof(INT32)*replicates));
  for (INT32 i = 0; i < replicates; ++i) {
    key[i] = i*2;
  }

  for (INT32 i = 1; i < replicates; ++i) {
    /*
    CHAR id[1024] = {0};
    ::snprintf(id, sizeof(id), "%s_%d", addr.c_str(), i);

    LONG key = Hash(id);
    NSF_HA_LOG_DEBUG("node cluster insert key:" << key << " to tree ...............................");
    */
    AddToTree(&rb_tree, key[i]);
    //NSF_HA_LOG_DEBUG("node cluster insert key:" << key[i] << " to tree ...............................");
  }
  /*
  INT32 deleted = replicates - 10;

  for (INT32 i = 0; i < deleted; ++i) {

    CHAR id[1024] = {0};
    ::snprintf(id, sizeof(id), "%s_%d", addr.c_str(), i);

    LONG key = Hash(id);
    NSF_HA_LOG_DEBUG("node cluster delete key:" << key << " from tree ...............................");

    DelFromTree(&rb_tree, key[i]);
    // NSF_HA_LOG_DEBUG("node cluster delete key:" << key[i] << " from tree ...............................");

    // NSF_HA_LOG_DEBUG("node cluster delete ....... tree size:" << rb_tree.size() << " root key:" << rb_tree.Root()->key_ << " ...............................");
  }
  */

  INT32 search[] = {1, 1, 3, 5, 1990, 1991, 1999};
  NSF_HA_LOG_DEBUG("node cluster tree size:" << rb_tree.size() << " root key:" << rb_tree.Root()->key_ << " ...............................");
  for (INT32 i = 1; i < 7; ++i) {
    RBTreeNode *node = rb_tree.upper_bound_first(String::ToString(search[i]));
    if (NULL != node) {
      NSF_HA_LOG_DEBUG("node cluster tree size:" << rb_tree.size() << " node key:" << node->key_
                                                 << " ...............................");
    }
  }
  rb_tree.TravelMid(Print, NULL);

  rb_tree.FreeTree();

  delete HandleManager::GetFrameInstance();

  return 0;
}
