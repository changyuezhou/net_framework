// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_LOG_H_
#define NSF_PUBLIC_INC_LOG_H_

#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/log/inc/log.h"

namespace nsf {
  namespace pub {
    #define NSF_PUBLIC_LOG_DEBUG(expression)  LOG_FRAME_DEBUG(64, expression)
    #define NSF_PUBLIC_LOG_INFO(expression)  LOG_FRAME_INFO(64, expression)
    #define NSF_PUBLIC_LOG_WARN(expression) LOG_FRAME_WARN(64, expression)
    #define NSF_PUBLIC_LOG_ERROR(expression) LOG_FRAME_ERROR(64, expression)
    #define NSF_PUBLIC_LOG_FATAL(expression) LOG_FRAME_FATAL(64, expression)

    #define NSF_AP_LOG_DEBUG(expression) LOG_FRAME_DEBUG(8, expression)
    #define NSF_AP_LOG_INFO(expression) LOG_FRAME_INFO(8, expression)
    #define NSF_AP_LOG_WARN(expression) LOG_FRAME_WARN(8, expression)
    #define NSF_AP_LOG_ERROR(expression) LOG_FRAME_ERROR(8, expression)
    #define NSF_AP_LOG_FATAL(expression) LOG_FRAME_FATAL(8, expression)

    #define NSF_JOB_LOG_DEBUG(expression) LOG_FRAME_DEBUG(16, expression)
    #define NSF_JOB_LOG_INFO(expression) LOG_FRAME_INFO(16, expression)
    #define NSF_JOB_LOG_WARN(expression) LOG_FRAME_WARN(16, expression)
    #define NSF_JOB_LOG_ERROR(expression) LOG_FRAME_ERROR(16, expression)
    #define NSF_JOB_LOG_FATAL(expression) LOG_FRAME_FATAL(16, expression)

    #define NSF_CTRL_LOG_DEBUG(expression) LOG_FRAME_DEBUG(32, expression)
    #define NSF_CTRL_LOG_INFO(expression) LOG_FRAME_INFO(32, expression)
    #define NSF_CTRL_LOG_WARN(expression) LOG_FRAME_WARN(32, expression)
    #define NSF_CTRL_LOG_ERROR(expression) LOG_FRAME_ERROR(32, expression)
    #define NSF_CTRL_LOG_FATAL(expression) LOG_FRAME_FATAL(32, expression)
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_LOG_H_