// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/job/inc/config.h"
#include "commlib/magic/inc/dir.h"
#include "commlib/magic/inc/crc.h"

namespace nsf {
  namespace job {
    using lib::magic::Dir;
    using lib::magic::CRC;

    INT32 Config::Loading(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "job.pipe.id");
      pipe_info_.job_id_ = config.GetINT32(key);
      ::snprintf(key, sizeof(key), "job.pipe.size");
      pipe_info_.size_ = config.GetINT32(key) << 20;
      ::snprintf(key, sizeof(key), "job.pipe.lock");
      pipe_info_.lock_ = config.GetBOOL(key);

      CHAR path[1024] = {0};
      ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*pipe_info_.job_id_);
      pipe_info_.producer_key_= CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));
      ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*pipe_info_.job_id_ + 1);
      pipe_info_.consumer_key_= CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));

      pipe_info_.Dump();

      INT32 result = LoadingParameters(config);
      if (0 != result) {
        return result;
      }

      result = LoadingPlugins(config);

      return result;
    }

    INT32 Config::LoadingParameters(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "job.wait.interval");
      loop_wait_time_ = config.GetINT32(key);
      if (0 >= loop_wait_time_) {
        loop_wait_time_ = 10;
      }

      ::snprintf(key, sizeof(key), "job.async.max.request");
      async_max_request_ = config.GetINT32(key);
      if (0 >= async_max_request_) {
        async_max_request_ = 1000;
      }

      return 0;
    }

    INT32 Config::LoadingPlugins(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "job.plugins.msg.handler.file");
      plugins_.msg_handler_file_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "job.plugins.msg.handler.etc");
      plugins_.msg_handler_etc_ = config.GetString(key);

      return 0;
    }

    const PipeConfInfo & Config::GetPipeInfo() {
      return pipe_info_;
    }

    const Config::Plugins & Config::GetPlugins() {
      return plugins_;
    }
  }  // namespace job
}  // namespace nsf