// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <string.h>
#include "nsf/plugins/emulate/inc/dynamicUI.h"
#include "commlib/magic/inc/singletonHolder.h"
#include "commlib/log/inc/handleManager.h"
#include "commlib/magic/inc/str.h"
#include "commlib/log/inc/log.h"

namespace nsf {
  namespace plugins {
    using lib::log::HandleManager;
    using lib::magic::SingletonHolder;
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

    INT32 MsgHandleInit(const CHAR * file) {
      if (0 != HandleManager::GetInstance()->Initial(file)) {
        printf("initial log file:%s failed\r\n", file);
        delete HandleManager::GetInstance();

        return -1;
      }

      return 0;
    }

    INT32 MsgHandleProcessing(const CHAR * data, INT32 size, CHAR * out, INT32 * out_size) {
      ::memcpy(out, data, size);
      *out_size = size;

      CHAR content[1024*2] = {0};
      ::memcpy(content, data, size);

      BOOL is_heart = FALSE;
      StringSplit split(content, "&");
      for (INT32 i = 0; i < split.size(); ++i) {
        StringSplit item(split[i], "=");
        if (0 == ::strncasecmp(item[0].c_str(), "is_heart", item[0].length())) {
          is_heart = TRUE;
        }
      }

      if (!is_heart) {
        LOG_TEST_INFO(content << " .......................................................................");
      }

      return 0;
    }

    INT32 MsgHandleFinish() {
      return 0;
    }
  }  // namespace plugins
}  // namespace nsf
