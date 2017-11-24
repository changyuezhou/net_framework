// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <unistd.h>
#include <signal.h>
#include <string>
#include <vector>
#include <map>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "commlib/magic/inc/dir.h"
#include "commlib/magic/inc/crc.h"
#include "commlib/magic/inc/daemonize.h"
#include "commlib/magic/inc/singletonHolder.h"
#include "commlib/log/inc/handleManager.h"
#include "nsf/job/inc/config.h"
#include "nsf/public/inc/channel.h"
#include "nsf/public/inc/monitor_cli.h"
#include "nsf/public/inc/pipe_comm.h"
#include "nsf/job/inc/event_dll.h"
#include "nsf/public/inc/acce.h"
#include "nsf/public/inc/err.h"

using std::pair;
using std::vector;
using lib::etc::conf;
using lib::magic::Dir;
using lib::magic::CRC;
using lib::magic::SingletonHolder;
using lib::magic::Daemon;
using lib::log::HandleManager;
using nsf::job::Config;
using nsf::pub::Channel;
using nsf::pub::Consumer;
using nsf::pub::Producer;
using nsf::pub::Err;
using nsf::job::EventDLL;
using nsf::pub::MonitorCli;
using nsf::pub::PipeConfInfo;
using std::string;

typedef pair<Consumer **, INT32> ConsumerResult;
typedef pair<Producer **, INT32> ProducerResult;
typedef vector<PipeConfInfo> PipeList;

typedef struct _write_func_params_item {
  Producer * producer_;
  INT32 ext_;
  CHAR  unique_[256];
} WriteFuncParamsItem;

typedef struct _write_func_params_items {
  INT32 size_;
  INT32 wptr_;
  WriteFuncParamsItem * items_;
} WriteFuncParamsItems;

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

INT32 WriteFunc(VOID * params, const CHAR * out, INT32 out_size) {
  if (NULL == params) {
    NSF_JOB_LOG_ERROR("send resp failed params is empty");
    return Err::kERR_JOB_ASYNC_WRITE_PARAMS_IS_INVALID;
  }

  WriteFuncParamsItem * item = reinterpret_cast<WriteFuncParamsItem *>(params);
  Producer * producer = item->producer_;
  if (NULL == producer) {
    NSF_JOB_LOG_ERROR("send resp failed producer params is empty");
    return Err::kERR_JOB_ASYNC_WRITE_PARAMS_IS_INVALID;
  }
  if (0 < out_size) {
    INT32 result = producer->Producing(out, out_size, item->ext_, item->unique_);
    if (0 != result) {
      NSF_JOB_LOG_ERROR("job write async resp failed result:" << result);
      return Err::kERR_JOB_ASYNC_WRITE_FAILED;
    }
  }

  return 0;
}

INT32 main(INT32 argc, CHAR ** argv) {
  if (2 != argc) {
    printf("Usage: nsf_job [config file]");
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
    NSF_JOB_LOG_ERROR("etc create failed config file:" << argv[1]);
    return -1;
  }

  Config config;
  if (0 != config.Loading(etc)) {
    NSF_JOB_LOG_ERROR("config file loading failed");
    return -1;
  }

  CHAR key[1024] = {0};
  ::snprintf(key, sizeof(key), "%s_%d", Dir::GetBinDir().c_str(), 255);
  INT32 mq_key = reinterpret_cast<UINT32>(CRC::CRC32(reinterpret_cast<UCHAR *>(key), ::strlen(key)));

  MonitorCli cli;
  if (0 != cli.CreateMQ(mq_key, config.GetPipeInfo().job_id_)) {
    NSF_JOB_LOG_ERROR("create message queue failed key:" << (UINT32)mq_key);
    return -1;
  }

  EventDLL dll;
  if (0 != dll.LoadingMsgHandler(config.GetPlugins().msg_handler_file_)) {
    NSF_JOB_LOG_ERROR("loading plugins file:" << config.GetPlugins().msg_handler_file_ << " failed");
    return -1;
  }

  INT32 init_result = 0;
  if (0 != (init_result = dll.MsgHandleInit(config.GetPlugins().msg_handler_etc_))) {
    NSF_JOB_LOG_ERROR("plugins initial etc:" << config.GetPlugins().msg_handler_etc_ << " failed code:" << init_result);
    return -1;
  }

  INT32 loop_wait_time = config.GetLoopWaitTime();

  Channel channel;
  PipeList list;
  list.push_back(config.GetPipeInfo());
  if (0 != channel.CreatePipeList(list)) {
    NSF_JOB_LOG_ERROR("channel create pipe failed");
    return -1;
  }

  ConsumerResult c_result = channel.GetConsumerList();
  ProducerResult p_result = channel.GetProducerList();
  Consumer * consumer = c_result.first[0];
  Producer * producer = p_result.first[0];

  INT32 size = config.GetPipeInfo().size_;
  CHAR * in = reinterpret_cast<CHAR *>(::malloc(size));
  CHAR * out = reinterpret_cast<CHAR *>(::malloc(size));
  if (NULL == in || NULL == out) {
    NSF_JOB_LOG_ERROR("malloc in and out failed size:" << size);
    return -1;
  }
/*
  WriteFuncParamsItems params_items;
  params_items.size_ = config.GetAsyncMaxRequest();
  if (1000 >= params_items.size_) {
    params_items.size_ = 1000;
  }
  params_items.wptr_ = 0;
  params_items.items_ = reinterpret_cast<WriteFuncParamsItem *>(::malloc(params_items.size_*sizeof(WriteFuncParamsItem)));
  if (NULL == params_items.items_) {
    NSF_JOB_LOG_ERROR("job malloc async write request params items failed");
    return -1;
  }
  memset(params_items.items_, 0x00, params_items.size_*sizeof(WriteFuncParamsItem));
*/
  WriteFuncParamsItem params_item;
  memset(&params_item, 0x00, sizeof(params_item));
  UINT32 last_report = ::time(NULL);
  if (0 != cli.Report()) {
    NSF_JOB_LOG_ERROR("job report status failed code:" << errno);
    return -1;
  }

  while (TRUE) {
    if (unlikely(!SingletonHolder<Process>::Instance()->GetRunning())) {
      break;
    }
    UINT32 now = ::time(NULL);
    if (unlikely(1 <= (now - last_report))) {
      last_report = now;
      if (0 != cli.Report()) {
        NSF_JOB_LOG_WARN("job report status failed code:" << errno);

        break;
      }
    }
    INT32 ext = 0;
    CHAR uniq[256] = {0};
    INT32 in_size = size;
    INT32 result = consumer->Consuming(in, &in_size, &ext, uniq, sizeof(uniq));
    if (0 == result) {
      INT32 out_size = size;
      params_item.producer_ = producer;
      params_item.ext_ = ext;
      ::memcpy(params_item.unique_, uniq, ::strlen(uniq));
      if (0 == dll.MsgHandleProcessing(in, in_size, out, &out_size, WriteFunc,
                                       reinterpret_cast<VOID *>(&params_item), sizeof(WriteFuncParamsItem))) {
        if (0 < out_size) {
          result = producer->Producing(out, out_size, ext, uniq);
          if (0 != result) {
            NSF_JOB_LOG_WARN("send resp failed result:" << result);
          }
        }
      } else {
        NSF_JOB_LOG_WARN("msg handler processing failed");
      }
    } else {
      if (Err::kERR_PIPE_COMM_BUFFER_EMPTY == result) {
        ::usleep(loop_wait_time);
      }
      // printf("consuming failed result:%d\r\n", result);
    }
  }

  if (in) {
    ::free(in);
  }

  if (out) {
    ::free(out);
  }

  NSF_JOB_LOG_INFO("************************************************************\n");

  delete SingletonHolder<Process>::Instance();
  delete HandleManager::GetFrameInstance();

  if (0 != dll.MsgHandleFinish()) {
    NSF_JOB_LOG_ERROR("plugins finish failed\n");
    return -1;
  }

  return 0;
}
