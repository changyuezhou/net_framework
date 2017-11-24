// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <unistd.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "commlib/log/inc/handleManager.h"
#include "nsf/job/inc/config.h"
#include "nsf/public/inc/channel.h"

using lib::etc::conf;
using nsf::job::Config;
using nsf::pub::Channel;

INT32 main(INT32 argc, CHAR ** argv) {
  conf etc;
  if (0 != etc.Create("./job.conf")) {
    printf("etc create failed\r\n");
    return -1;
  }

  Config config;
  if (0 != config.Loading(etc)) {
    printf("config loading failed\r\n");
    return -1;
  }

  Channel channel;
  if (0 != channel.CreatePipeList(config.GetPipeInfo())) {
    printf("channel create pipe failed\r\n");
    return -1;
  }

  while (TRUE) {
    channel.Dump();
    ::sleep(1);
  }

  return 0;
}