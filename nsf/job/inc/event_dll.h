// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_JOB_INC_EVENT_DLL_H_
#define NSF_JOB_INC_EVENT_DLL_H_

#include <stdlib.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/magic/inc/dll.h"

namespace nsf {
  namespace job {
    using std::string;
    using lib::magic::DLL;

    typedef INT32 (*_MsgHandleInit)(const CHAR * file);
    typedef INT32 (*AsyncCallBackWrite)(VOID * params, const CHAR * out, INT32 out_size);
    typedef INT32 (*_MsgHandleProcessing)(const CHAR * data, INT32 size, CHAR * out, INT32 * out_size,
                                          AsyncCallBackWrite func, VOID * params, INT32 params_size);
    typedef INT32 (*_MsgHandleFinish)();

    class EventDLL {
     public:
       EventDLL() {}
       ~EventDLL() {}

     public:
       INT32 LoadingMsgHandler(const string & file);

     public:
       INT32 MsgHandleInit(const string & file);
       INT32 MsgHandleProcessing(const CHAR * data, INT32 size, CHAR * out, INT32 * out_size,
                                 AsyncCallBackWrite func, VOID * params, INT32 params_size);
       INT32 MsgHandleFinish();

     private:
       _MsgHandleInit msg_handler_init_;
       _MsgHandleProcessing msg_handler_processing_;
       _MsgHandleFinish msg_handler_finish_;
       DLL dll_;
    };
  }  // namespace job
}  // namespace nsf

#endif  // NSF_JOB_INC_EVENT_DLL_H_
