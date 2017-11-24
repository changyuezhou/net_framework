// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PLUGINS_HASH_INC_HASH_H_
#define NSF_PLUGINS_HASH_INC_HASH_H_

#include <stdlib.h>
#include <string>
#include "commlib/public/inc/type.h"

namespace nsf {
  namespace plugins {
    extern "C" {
    INT32 TcpHandleInit(const CHAR * file);
    INT32 TcpHandlerConnected(INT32 h, const CHAR * ip, INT32 port);
    INT32 TcpHandlerDisconnected(INT32 h, const CHAR * ip, INT32 port);
    INT32 FinishTcpHandle();

    INT32 MsgHandleUnpack(const CHAR * data, INT32 size);
    INT32 MsgHandleRoute(const CHAR * data, INT32 size);

    INT32 Hash(const CHAR * id);
    INT32 DecodeUpPackage(const CHAR * data, INT32 size, CHAR * id, INT32 * id_size, CHAR * seq, INT32 * seq_size);
    INT32 DecodeDownPackage(const CHAR * data, INT32 size, CHAR * seq, INT32 * seq_size, INT32 * is_heart);
    INT32 EncodeHeart(CHAR * data, INT32 * siz);
    }
  }  // namespace plugins
}  // namespace nsf

#endif  // NSF_PLUGINS_HASH_INC_HASH_H_
