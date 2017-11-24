// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <string.h>
#include "nsf/ha/inc/config.h"
#include "nsf/ha/inc/log.h"
#include "commlib/magic/inc/dir.h"
#include "commlib/magic/inc/crc.h"

namespace nsf {
  namespace ha {
    using lib::magic::Dir;
    using lib::magic::CRC;

    INT32 Config::Loading(const string & file) {
      conf etc;
      if (0 != etc.Create(file)) {
        NSF_HA_LOG_ERROR("ha create config file failed:" << file);

        return -1;
      }

      if (0 != Loading(etc)) {
        NSF_HA_LOG_ERROR("config file loading failed");

        return -1;
      }

      return 0;
    }

    INT32 Config::Loading(const conf & config) {
      INT32 result = LoadingPipeInfo(config);
      if (0 != result) {
        NSF_HA_LOG_ERROR("config loading pipe info failed");

        return result;
      }

      result = LoadingConnectPool(config);
      if (0 != result) {
        NSF_HA_LOG_ERROR("config loading connect pool failed");

        return result;
      }

      result = LoadingCenterAndRoute(config);
      if (0 != result) {
        NSF_HA_LOG_ERROR("config loading center and route failed");

        return result;
      }

      return 0;
    }

    INT32 Config::LoadingPipeInfo(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "ha.pipe.id");
      pipe_info_.job_id_ = config.GetINT32(key);
      ::snprintf(key, sizeof(key), "ha.pipe.size");
      pipe_info_.size_ = config.GetINT32(key) << 20;
      ::snprintf(key, sizeof(key), "ha.pipe.lock");
      pipe_info_.lock_ = config.GetBOOL(key);

      CHAR path[1024] = {0};
      ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*pipe_info_.job_id_);
      pipe_info_.producer_key_= CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));
      ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*pipe_info_.job_id_ + 1);
      pipe_info_.consumer_key_= CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));

      return 0;
    }

    INT32 Config::LoadingConnectPool(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "ha.connection.min");
      connect_pool_min_ = config.GetINT32(key);
      if (0 >= connect_pool_min_) {
        connect_pool_min_ = 1;
      }
      ::snprintf(key, sizeof(key), "ha.connection.max");
      connect_pool_max_ = config.GetINT32(key);
      if (0 >= connect_pool_max_) {
        connect_pool_max_ = 1;
      }
      ::snprintf(key, sizeof(key), "ha.connection.timeout");
      connect_pool_timeout_ = config.GetINT32(key);
      if (0 >= connect_pool_timeout_) {
        connect_pool_timeout_ = 10;
      }
      ::snprintf(key, sizeof(key), "ha.connection.heart.timeout");
      heart_timeout_ = config.GetINT32(key);
      if (0 >= heart_timeout_) {
        heart_timeout_ = 1;
      }

      ::snprintf(key, sizeof(key), "ha.connection.threshold");
      busy_threshold_ = config.GetINT32(key);
      if (0 >= busy_threshold_) {
        busy_threshold_ = 50;
      }

      return 0;
    }

    INT32 Config::LoadingCenterAndRoute(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "ha.route.file");
      hash_file_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "ha.route.reg");
      const string & reg = config.GetString(key);
      if (0 == ::strncasecmp("ROUND_ROBIN", reg.c_str(), reg.length())) {
        route_ = 1;
      } else if (0 == ::strncasecmp("CONSISTENT_HASH", reg.c_str(), reg.length())) {
        route_ = 2;
      } else {
        route_ = 1;
      }
      ::snprintf(key, sizeof(key), "ha.center.addr");
      center_addr_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "ha.center.lb.addr");
      lb_addr_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "ha.route.replicates");
      replicates_ = config.GetINT32(key);
      if (0 >= replicates_) {
        replicates_ = 10;
      }

      return 0;
    }
  }  // namespace ha
}  // namespace nsf