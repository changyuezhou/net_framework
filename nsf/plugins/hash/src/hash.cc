// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <string.h>
#include "nsf/plugins/hash/inc/hash.h"
#include "commlib/magic/inc/str.h"

namespace nsf {
  namespace plugins {
    using lib::magic::StringSplit;

    INT32 TcpHandleInit(const CHAR * file) {
      return 0;
    }

    INT32 TcpHandlerConnected(INT32 h, const CHAR * ip, INT32 port) {
      return 0;
    }

    INT32 TcpHandlerDisconnected(INT32 h, const CHAR * ip, INT32 port) {
      return 0;
    }

    INT32 FinishTcpHandle() {
      return 0;
    }

    INT32 MsgHandleUnpack(const CHAR * data, INT32 size) {
      return size;
    }

    INT32 MsgHandleRoute(const CHAR * data, INT32 size) {
      return 0;
    }

    INT32 Hash(const CHAR * id) {
      return 0;
    }

    INT32 DecodeUpPackage(const CHAR * data, INT32 size, CHAR * id, INT32 * id_size, CHAR * seq, INT32 * seq_size) {
      StringSplit split(data, "&");
      for (INT32 i = 0; i < split.size(); ++i) {
        StringSplit item(split[i], "=");
        if (0 == ::strncasecmp(item[0].c_str(), "id", item[0].length())) {
          ::snprintf(id, *id_size, "%s", item[1].c_str());

          *id_size = item[1].length();
        }

        if (0 == ::strncasecmp(item[0].c_str(), "seq", item[0].length())) {
          ::snprintf(seq, *seq_size, "%s", item[1].c_str());

          *seq_size = item[1].length();
        }
      }
      return 0;
    }

    INT32 DecodeDownPackage(const CHAR * data, INT32 size, CHAR * seq, INT32 * seq_size, INT32 * is_heart) {
      StringSplit split(data, "&");
      for (INT32 i = 0; i < split.size(); ++i) {
        StringSplit item(split[i], "=");
        if (0 == ::strncasecmp(item[0].c_str(), "is_heart", item[0].length())) {
          *is_heart = ::atoi(item[1].c_str());
        }

        if (0 == ::strncasecmp(item[0].c_str(), "seq", item[0].length())) {
          ::snprintf(seq, *seq_size, "%s", item[1].c_str());

          *seq_size = item[1].length();
        }
      }

      return 0;
    }

    INT32 EncodeHeart(CHAR * data, INT32 * size) {
      ::snprintf(data, *size, "seq=123456&is_heart=1");
      *size = ::strlen("seq=123456&is_heart=1");

      return 0;
    }
  }  // namespace plugins
}  // namespace nsf
