// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/public/inc/log.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/acce.h"
#include "nsf/job/inc/event_dll.h"

namespace nsf {
  namespace job {
    INT32 EventDLL::LoadingMsgHandler(const string & file) {
      INT32 result = dll_.CreateDll(file);
      if (0 != result) {
        return result;
      }

      msg_handler_init_ = reinterpret_cast<_MsgHandleInit>(dll_.GetFunctionHandle("MsgHandleInit"));
      msg_handler_processing_ = reinterpret_cast<_MsgHandleProcessing>(dll_.GetFunctionHandle("MsgHandleProcessing"));
      msg_handler_finish_ = reinterpret_cast<_MsgHandleFinish>(dll_.GetFunctionHandle("MsgHandleFinish"));

      return 0;
    }

    INT32 EventDLL::MsgHandleInit(const string & file) {
      if (NULL != msg_handler_init_) {
        return msg_handler_init_(file.c_str());
      }

      NSF_JOB_LOG_DEBUG("event dll initial msg handler success");

      return 0;
    }

    INT32 EventDLL::MsgHandleProcessing(const CHAR * data, INT32 size, CHAR * out, INT32 * out_size,
                                        AsyncCallBackWrite func, VOID * params, INT32 params_size) {
      if (NULL != msg_handler_processing_) {
        return msg_handler_processing_(data, size, out, out_size, func, params, params_size);
      }

      NSF_JOB_LOG_DEBUG("event dll processing msg handler success");

      return 0;
    }

    INT32 EventDLL::MsgHandleFinish() {
     if (NULL != msg_handler_finish_) {
        return msg_handler_finish_();
      }

      NSF_AP_LOG_DEBUG("event dll finish msg handler success");

      return 0;
    }
  }  // namespace job
}  // namespace nsf
