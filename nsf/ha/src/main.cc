// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <unistd.h>
#include <signal.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/magic/inc/daemonize.h"
#include "commlib/magic/inc/singletonHolder.h"
#include "commlib/log/inc/handleManager.h"
#include "nsf/ha/inc/ha.h"
#include "nsf/ha/inc/log.h"

using lib::magic::Daemon;
using lib::magic::SingletonHolder;
using lib::log::HandleManager;
using nsf::ha::HA;

VOID SigHandler(INT32 signo) {
  if (SIGUSR1 == signo) {
    SingletonHolder<HA>::Instance()->Stop();
  }
}

INT32 main(INT32 argc, CHAR ** argv) {
  if (2 != argc) {
    printf("Usage: ha [config file]\r\n");
    return -1;
  }

  if (0 != Daemon::Daemonize(1, 1, FALSE)) {
    printf("deamon failed\n");
    return -1;
  }

  ::signal(SIGUSR1, SigHandler);

  if (0 != HandleManager::GetFrameInstance()->Initial(argv[1])) {
    printf("initial log file:%s failed\r\n", argv[1]);
    delete HandleManager::GetFrameInstance();

    return -1;
  }

  if (0 != SingletonHolder<HA>::Instance()->Initial(argv[1])) {
    NSF_HA_LOG_ERROR("ha initial failed");

    delete HandleManager::GetFrameInstance();
    delete SingletonHolder<HA>::Instance();
    return -1;
  }

  SingletonHolder<HA>::Instance()->DoEvent();

  ::usleep(2000000);

  delete SingletonHolder<HA>::Instance();
  delete HandleManager::GetFrameInstance();

  return 0;
}
