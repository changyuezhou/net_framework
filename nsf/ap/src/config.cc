// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/ap/inc/config.h"
#include "nsf/public/inc/err.h"
#include "commlib/magic/inc/dir.h"
#include "commlib/magic/inc/crc.h"
#include "commlib/magic/inc/str.h"


namespace nsf {
  namespace ap {
    using lib::magic::Dir;
    using lib::magic::CRC;
    using lib::magic::StringSplit;

    INT32 Config::Loading(const conf & config) {
      INT32 result = LoadingService(config);
      if (0 != result) {
        return result;
      }

      result = LoadingPipe(config);
      if (0 != result) {
        return result;
      }

      result = LoadingParameters(config);
      if (0 != result) {
        return result;
      }

      result = LoadingPlugins(config);
      return result;
    }

    INT32 Config::LoadingService(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "net.service.size");
      INT32 size = config.GetINT32(key);

      for (INT32 i = 0; i < size; ++i) {
        ServiceItem item;
        ::snprintf(key, sizeof(key), "net.service%d.type", i);
        item.type_ = config.GetString(key);
        ::snprintf(key, sizeof(key), "net.service%d.addr", i);
        item.addr_ = config.GetString(key);
        ::snprintf(key, sizeof(key), "net.service%d.tos", i);
        item.tos_ = config.GetINT32(key);
        ::snprintf(key, sizeof(key), "net.service%d.max.fd", i);
        item.max_fd_ = config.GetINT32(key);
        ::snprintf(key, sizeof(key), "net.service%d.backlog", i);
        item.backlog_ = config.GetINT32(key);
        ::snprintf(key, sizeof(key), "net.service%d.timeout", i);
        item.timeout_ = config.GetINT32(key);
        ::snprintf(key, sizeof(key), "net.service%d.recv.size", i);
        item.recv_size_ = config.GetINT32(key);
        ::snprintf(key, sizeof(key), "net.service%d.send.size", i);
        item.send_size_ = config.GetINT32(key);
        ::snprintf(key, sizeof(key), "net.service%d.keepalive", i);
        item.keep_alive_ = config.GetBOOL(key);

        service_list_.push_back(item);
      }

      return 0;
    }

    INT32 Config::LoadingPipe(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "net.pipe.size");
      INT32 size = config.GetINT32(key);

      for (INT32 i = 0; i < size; ++i) {
        PipeConfInfo item;
        ::snprintf(key, sizeof(key), "net.pipe%d.id", i);
        item.job_id_ = config.GetINT32(key);
        ::snprintf(key, sizeof(key), "net.pipe%d.size", i);
        item.size_ = config.GetINT32(key) << 20;
        ::snprintf(key, sizeof(key), "net.pipe%d.lock", i);
        item.lock_ = config.GetBOOL(key);

        CHAR path[1024] = {0};
        ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*item.job_id_);
        item.consumer_key_= CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));
        ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*item.job_id_ + 1);
        item.producer_key_= CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));

        item.Dump();
        pipe_list_.push_back(item);
      }

      return 0;
    }

    INT32 Config::LoadingParameters(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "net.wait.interval");
      wait_loop_milliseconds_ = config.GetINT32(key);
      if (0 == wait_loop_milliseconds_) {
        wait_loop_milliseconds_ = 10;
      }

      ::snprintf(key, sizeof(key), "net.id");
      group_id_ = config.GetINT32(key);
      if (0 == group_id_) {
        group_id_ = 1;
      }      

      ::snprintf(key, sizeof(key), "net.filter.reg");
      const string & reg = config.GetString(key);
      FILTER_REG filter_reg = BLACK;
      if (0 == ::strncasecmp(reg.c_str(), "white", reg.length())) {
        filter_reg = WHITE;
      }
      ::snprintf(key, sizeof(key), "net.filter.list");
      const string & ip_list = config.GetString(key);
      if (ip_list.length()) {
        StringSplit split(ip_list, ",");
        INT32 size = split.TokenSize();
        for (INT32 i = 0; i < size; ++i) {
          const string & item = split.GetToken(i);
          if (service_list_.size()) {
            ServiceList::iterator it = service_list_.begin();
            while (it != service_list_.end()) {
              it->ip_list_.push_back(item.substr(0, item.find_first_of("*")));
              it++;
            }
          }
        }

        if (service_list_.size()) {
          ServiceList::iterator it = service_list_.begin();
          while (it != service_list_.end()) {
            it->filter_reg_ = filter_reg;
            sort(it->ip_list_.begin(), it->ip_list_.end());
            it++;
          }
        }
      }

      return 0;
    }

    INT32 Config::LoadingPlugins(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "plugins.msg.handler.file");
      plugins_.msg_handler_file_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "plugins.msg.handler.etc");
      plugins_.msg_handler_etc_ = config.GetString(key);

      return 0;
    }

    const Config::ServiceList & Config::GetServiceList() {
      return service_list_;
    }

    const Config::PipeList & Config::GetPipeList() {
      return pipe_list_;
    }

    const Config::Plugins & Config::GetPlugins() {
      return plugins_;
    }
  }  // namespace ap
}  // namespace nsf