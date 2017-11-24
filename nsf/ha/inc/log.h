// Copyright (c) 2016 fonova. All rights reserved.

#ifndef NSF_HA_INC_LOG_H_
#define NSF_HA_INC_LOG_H_

#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/log/inc/log.h"

namespace nsf {
    namespace ha {
      #define NSF_HA_LOG_DEBUG(expression)  LOG_FRAME_DEBUG(128, expression)
      #define NSF_HA_LOG_INFO(expression)  LOG_FRAME_INFO(128, expression)
      #define NSF_HA_LOG_WARN(expression) LOG_FRAME_WARN(128, expression)
      #define NSF_HA_LOG_ERROR(expression) LOG_FRAME_ERROR(128, expression)
    }  // namespace ha
}  // namespace nsf

#endif  // NSF_HA_INC_LOG_H_