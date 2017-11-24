// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <unistd.h>
#include <signal.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "commlib/magic/inc/daemonize.h"
#include "commlib/magic/inc/singletonHolder.h"
#include "commlib/log/inc/handleManager.h"
#include "nsf/public/inc/monitor_svr.h"
#include "nsf/public/inc/acce.h"

using lib::etc::conf;
using lib::magic::Daemon;
using lib::magic::SingletonHolder;
using lib::log::HandleManager;
using nsf::pub::MonitorSvr;

class Process {
 public:
   Process(): running_(TRUE) {}
   ~Process() {}

 public:
   VOID SetRunning(BOOL r) { running_ = r; }
   BOOL GetRunning() { return running_; }

 private:
   BOOL running_;
};

VOID SigHandler(INT32 signo) {
  if (SIGUSR1 == signo) {
    SingletonHolder<Process>::Instance()->SetRunning(FALSE);
  }
}

INT32 main(INT32 argc, CHAR ** argv) {
  if (2 != argc) {
    printf("Usage: nsf_ctrl [config file]");
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
    NSF_CTRL_LOG_ERROR("etc create file: " << argv[1] << " failed");
    return -1;
  }

  MonitorSvr monitor_svr;
  INT32 result = monitor_svr.Create(etc);
  if (0 != result) {
    NSF_CTRL_LOG_ERROR("create monitor service failed code:" << result);
    return -1;
  }

  result = monitor_svr.Initial();
  if (0 != result) {
    NSF_CTRL_LOG_ERROR("ctrl monitor service check failed code:" << result);
  }

  ::usleep(1000000);

  monitor_svr.Dump();

  while (TRUE) {
    if (unlikely(!SingletonHolder<Process>::Instance()->GetRunning())) {
      break;
    }

    monitor_svr.Running();

    ::usleep(100000);
  }

  monitor_svr.KillAll();

  ::usleep(2000000);

  delete SingletonHolder<Process>::Instance();
  delete HandleManager::GetFrameInstance();

  NSF_CTRL_LOG_INFO("************************************************************\n");

  return 0;
}
