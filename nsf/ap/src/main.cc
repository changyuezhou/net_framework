// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <unistd.h>
#include <signal.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "commlib/magic/inc/daemonize.h"
#include "commlib/magic/inc/singletonHolder.h"
#include "nsf/ap/inc/reactor.h"
#include "nsf/ap/inc/ep_reactor.h"
#include "nsf/ap/inc/config.h"
#include "commlib/log/inc/handleManager.h"

using lib::etc::conf;
using lib::magic::SingletonHolder;
using lib::magic::Daemon;
using lib::log::HandleManager;
using nsf::ap::Config;
using nsf::ap::EPReactor;
using nsf::ap::Reactor;

VOID SigHandler(INT32 signo) {
  if (SIGUSR1 == signo) {
    SingletonHolder<EPReactor>::Instance()->SetRunning(FALSE);
  }
}

INT32 main(INT32 argc, CHAR ** argv) {
  if (2 != argc) {
    printf("Usage: nsf_ap [config file]");
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

  conf etc;
  if (0 != etc.Create(argv[1])) {
    NSF_AP_LOG_ERROR("etc create failed config file:" << argv[1]);
    return -1;
  }

  Config config;
  if (0 != config.Loading(etc)) {
    NSF_AP_LOG_ERROR("config loading failed");
    return -1;
  }

  Reactor * reactor = SingletonHolder<EPReactor>::Instance();
  if (NULL == reactor) {
    NSF_AP_LOG_ERROR("new epoll reactor object failed");
    return -1;
  }

  INT32 result = 0;
  if (0 != (result = reactor->Create(config.GetServiceList(), config.GetPipeList(), config.GetPlugins()))) {
    NSF_AP_LOG_ERROR("epoll reactor create failed code:" << result);
    return -1;
  }

  reactor->SetGroupID(config.GetGroupID());

  reactor->HandleEvents(config.GetLoopWait());

  delete reactor;
  delete HandleManager::GetFrameInstance();

  NSF_AP_LOG_INFO("****************************************************");

  return 0;
}