// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_ERR_H_
#define NSF_PUBLIC_INC_ERR_H_

#include <errno.h>
#include <string>
#include <cstdio>
#include "commlib/public/inc/type.h"

namespace nsf {
  namespace pub {
    using std::string;

    class Err {
     public:
       static const INT32 kERR_EPOLL_CTRL_FAILED = -101000;
       static const INT32 kERR_EPOLL_CREATE_EVENTS_POOL_FAILED = -101001;
       static const INT32 kERR_EPOLL_CREATE_FD_FAILED = -101002;
       static const INT32 kERR_SOCKET_HANDLER_CREATE_CONFIG = -101003;
       static const INT32 kERR_SERVICE_CREATE_FAILED = -101004;

       static const INT32 kERR_DGRAM_ACCEPTOR_HANDLER_POOLS_EMPTY = -101005;
       static const INT32 kERR_UDP_HANDLER_READ_FAILED = -101006;
       static const INT32 kERR_STREAM_ACCEPTOR_HANDLER_POOLS_EMPTY = -101007;
       static const INT32 kERR_TCP_HANDLER_READ_FAILED = -101008;
       static const INT32 kERR_STREAM_ACCEPTOR_HANDLER_EMPTY = -101009;
       static const INT32 kERR_HANDLER_CREATE_FAILED = -101010;
       static const INT32 kERR_ACCEPTOR_BUILDING_FAILED = -101012;
       static const INT32 kERR_HANDLER_BUILDING_FAILED = -101013;
       static const INT32 kERR_EPOLL_CREATE_FAILED = -101014;
       static const INT32 kERR_EPOLL_REACTOR_HANDLER_INVALID = -101015;
       static const INT32 kERR_EPOLL_REACTOR_CREATE_CHANNEL_FAILED = -101016;
       static const INT32 kERR_EPOLL_REACTOR_MALLOC_RECEIVE_BUFFER_FAILED = -101017;
       static const INT32 kERR_STREAM_ACCEPTOR_HANDLER_POOLS_FULL = -101018;

       static const INT32 kERR_PIPE_COMM_RT_INFO_INVALID = -101020;
       static const INT32 kERR_PIPE_COMM_BUFFER_FULL = -101021;
       static const INT32 kERR_PIPE_COMM_BUFFER_EMPTY = -101022;
       static const INT32 kERR_PIPE_COMM_DATA_TAG_INVALID = -101023;
       static const INT32 kERR_PIPE_COMM_SHARED_MEMORY_AT_FAILED = -101024;
       static const INT32 kERR_PIPE_COMM_SHARED_MEMORY_GET_FAILED = -101025;
       static const INT32 kERR_PIPE_COMM_FILE_LOCK_CREATE_FAILED = -101026;
       static const INT32 kERR_PIPE_COMM_FILE_LOCK_FAILED = -101027;
       static const INT32 kERR_PIPE_COMM_FILE_UNLOCK_FAILED = -101028;
       static const INT32 kERR_PIPE_COMM_UNIQ_LENGTH_LARGER = -101029;
       static const INT32 kERR_CONSUMER_PIPE_INVALID = -101029;
       static const INT32 kERR_PRODUCER_PIPE_INVALID = -101030;
       static const INT32 kERR_CHANNEL_PIPE_LIST_EMPTY = -101031;
       static const INT32 kERR_CHANNEL_CONSUMER_PRODUCER_CREATE_FAILED = -101032;
       static const INT32 kERR_CHANNEL_CONSUMER_MALLOC_FAILED = -101033;
       static const INT32 kERR_CHANNEL_PRODUCER_MALLOC_FAILED = -101034;
       static const INT32 kERR_CHANNEL_CONSUMER_CREATE_FAILED = -101035;
       static const INT32 kERR_CHANNEL_PRODUCER_CREATE_FAILED = -101036;
       static const INT32 kERR_CHANNEL_RECEIVE_NO_DATA = -101037;
       static const INT32 kERR_CHANNEL_SEND_FAILED = -101038;
       static const INT32 kERR_CONSUMER_PIPE_BUFFER_TOO_SMALLER = -101039;
       static const INT32 kERR_CHANNEL_CONSUMER_LIST_CREATE_FAILED = -101040;
       static const INT32 kERR_CHANNEL_PRODUCER_LIST_CREATE_FAILED = -101041;
       static const INT32 kERR_CHANNEL_PRODUCER_LIST_EMPTY = -101042;
       static const INT32 kERR_CHANNEL_PRODUCER_NOT_FOUND = -101043;
       static const INT32 kERR_CHANNEL_PRODUCER_ALL_FULL = -101044;
       static const INT32 kERR_CHANNEL_PRODUCER_INDEX_INVALID = -101045;
       static const INT32 kERR_CHANNEL_PRODUCER_DOING_FAILED = -101046;

       static const INT32 kERR_AP_CONFIG_CREATE_KEY_FAILED = -101050;

       static const INT32 kERR_HANDLER_REACTOR_OBJECT_INVALID = -101060;
       static const INT32 kERR_HANDLER_CHANNEL_OBJECT_INVALID = -101061;

       static const INT32 kERR_MQ_COMM_CREATE_FAILED = -101070;
       static const INT32 kERR_MQ_COMM_HANDLER_INVALID = -101071;

       static const INT32 kERR_CTRL_JOBS_COUNT_INVALID = -101080;
       static const INT32 kERR_CTRL_TOOLS_COUNT_INVALID = -101081;
       static const INT32 kERR_CTRL_DO_FORK_BIN_FILE_NOT_EXISTS = -101082;
       static const INT32 kERR_CTRL_DO_FORK_ETC_FILE_NOT_EXISTS = -101083;
       static const INT32 kERR_CTRL_DO_FORK_SET_MASK_FAILED = -101084;
       static const INT32 kERR_CTRL_DO_RECV_GROUP_OBJECT_INVALID = -101085;
       static const INT32 kERR_CTRL_LOAD_AP_MALLOC_OBJECT_FAILED = -101086;
       static const INT32 kERR_CTRL_DO_UPDATE_GROUP_OBJECT_INVALID = -101087;
       static const INT32 kERR_CTRL_DO_KILL_PROCESS_FAILED = -101087;
       static const INT32 kERR_CTRL_MESSAGE_QUEUE_EXISTS = -101088;
       static const INT32 kERR_CTRL_MESSAGE_QUEUE_CREATE_FAILED = -101088;

       static const INT32 kERR_JOB_ASYNC_WRITE_PARAMS_IS_INVALID = -101100;
       static const INT32 kERR_JOB_ASYNC_WRITE_FAILED = -101101;
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_ERR_H_
