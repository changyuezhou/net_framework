// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <sched.h>
#include <sys/types.h>
#include <signal.h>
#include "nsf/public/inc/monitor_svr.h"
#include "commlib/magic/inc/dir.h"
#include "commlib/magic/inc/crc.h"

namespace nsf {
  namespace pub {
    using std::dec;
    using std::hex;
    using lib::magic::Dir;
    using lib::magic::CRC;

    INT32 MonitorSvr::Create(const conf & config) {
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "%s_%d", Dir::GetBinDir().c_str(), 255);
      INT32 mq_key = reinterpret_cast<UINT32>(CRC::CRC32(reinterpret_cast<UCHAR *>(key), ::strlen(key)));

      NSF_PUBLIC_LOG_DEBUG("monitor service mq_key:" << mq_key << " ..................................");

      if (0 == mq_comm_.Attach(mq_key)) {
        NSF_PUBLIC_LOG_ERROR("monitor service's message queue is exists id:0x" << hex << mq_key << dec);
        return Err::kERR_CTRL_MESSAGE_QUEUE_EXISTS;
      }

      if (0 != mq_comm_.CreateMQ(mq_key)) {
        NSF_PUBLIC_LOG_ERROR("monitor service's message queue create failed id:0x" << hex << mq_key << dec);
        return Err::kERR_CTRL_MESSAGE_QUEUE_CREATE_FAILED;
      }

      INT32 result = CreateAP(LoadAPItem(config));
      if (0 != result) {
        return result;
      }

      ProcessList jobs;
      ::snprintf(key, sizeof(key), "ctrl.jobs.pipe.size");
      INT32 job_size = config.GetINT32(key);
      for (INT32 i = 0; i < job_size; ++i) {
        jobs.push_back(LoadJobItem(config, i));
      }
      result = CreateJobs(jobs);
      if (0 != result) {
        return result;
      }

      ProcessList tools;
      ::snprintf(key, sizeof(key), "ctrl.tools.items.size");
      INT32 tool_size = config.GetINT32(key);
      for (INT32 i = 0; i < tool_size; ++i) {
        tools.push_back(LoadToolItem(config, i));
      }
      result = CreateTools(tools);
      if (0 != result) {
        return result;
      }

      return mq_comm_.CreateMQ(mq_key);
    }

    const MonitorSvr::ProcessItem MonitorSvr::LoadAPItem(const conf & config) {
      ProcessItem ap_item;

      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "ctrl.ap.groupid");
      ap_item.id_ = config.GetINT32(key);
      ::snprintf(key, sizeof(key), "ctrl.ap.bin");
      ap_item.bin_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "ctrl.ap.etc");
      ap_item.etc_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "ctrl.ap.count");
      ap_item.count_ = config.GetINT32(key);
      ::snprintf(key, sizeof(key), "ctrl.ap.interval");
      ap_item.active_interval_ = config.GetINT32(key);

      return ap_item;
    }

    INT32 MonitorSvr::LoadAP(const ProcessItem & conf) {
      APMETA * ap_meta = &ap_.meta_;
      ::snprintf(ap_meta->bin_file_, sizeof(ap_meta->bin_file_), "%s", conf.bin_.c_str());
      ::snprintf(ap_meta->etc_file_, sizeof(ap_meta->etc_file_), "%s", conf.etc_.c_str());
      ap_meta->id_ = conf.id_;
      ap_meta->heartbeat_ = conf.active_interval_;
      ap_meta->proc_count_ = conf.count_;

      CHAR path[1024] = {0};
      ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*ap_meta->id_);
      UINT32 c_key = CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));
      ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*ap_meta->id_ + 1);
      UINT32 p_key = CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));

      ap_meta->consumer_key_ = c_key;
      ap_meta->consumer_size_ = conf.size_;
      ap_meta->producer_key_ = p_key;
      ap_meta->producer_size_ = conf.size_;

      INT32 total_proc = ap_meta->proc_count_;
      APSTATUS * ap_status = &ap_.status_;
      ap_status->id_ = ap_meta->id_;
      ap_status->total_proc_count_ = total_proc;
      ap_status->alive_proc_count_ = 0;
      ap_status->dead_proc_count_ = 0;
      ProcStatus ** status_list = reinterpret_cast<ProcStatus **>(::malloc(sizeof(ProcStatus *) * total_proc));
      if (NULL == status_list) {
        NSF_PUBLIC_LOG_ERROR("ctrl monitor service load ap failed msg: malloc array object of status failed");
        return Err::kERR_CTRL_LOAD_AP_MALLOC_OBJECT_FAILED;
      }
      for (INT32 i = 0; i < total_proc; ++i) {
        ProcStatus * status = reinterpret_cast<ProcStatus *>(::malloc(sizeof(ProcStatus)));
        if (NULL == status) {
          NSF_PUBLIC_LOG_ERROR("ctrl monitor service load ap failed msg: malloc status object of status failed");
          return Err::kERR_CTRL_LOAD_AP_MALLOC_OBJECT_FAILED;
        } 
        ::memset(status, 0x00, sizeof(ProcStatus));
        status_list[i] = status;   
      }
      ap_status->proc_status_ = status_list;

      return 0;
    }

    INT32 MonitorSvr::CreateAP(const ProcessItem & conf) {
      INT32 result = LoadAP(conf);
      if (0 != result) {
        return result;
      }

      return 0;
    }

    const MonitorSvr::ProcessItem MonitorSvr::LoadJobItem(const conf & config, INT32 index) {
      ProcessItem item;
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "ctrl.jobs.pipe%d.groupid", index);
      item.id_ = config.GetINT32(key);
      ::snprintf(key, sizeof(key), "ctrl.jobs.pipe%d.bin", index);
      item.bin_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "ctrl.jobs.pipe%d.etc", index);
      item.etc_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "ctrl.jobs.pipe%d.count", index);
      item.count_ = config.GetINT32(key);
      ::snprintf(key, sizeof(key), "ctrl.jobs.pipe%d.interval", index);
      item.active_interval_ = config.GetINT32(key);

      return item;
    }

    INT32 MonitorSvr::LoadJobs(const ProcessList & conf) {
      INT32 size = conf.size();
      if (0 >= size) {
        return 0;
      }

      if (JOB_MAX_COUNT < size) {
        return Err::kERR_CTRL_JOBS_COUNT_INVALID;
      }

      ProcessList::const_iterator it = conf.begin();
      INT32 index = 0;
      while (conf.end() != it) {
        JOBMETA * job = &jobs_[index].meta_;
        ::snprintf(job->bin_file_, sizeof(job->bin_file_), "%s", it->bin_.c_str());
        ::snprintf(job->etc_file_, sizeof(job->etc_file_), "%s", it->etc_.c_str());
        job->id_ = it->id_;
        job->heartbeat_ = it->active_interval_;
        job->proc_count_ = it->count_;

        CHAR path[1024] = {0};
        ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*job->id_);
        UINT32 c_key = CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));
        ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*job->id_ + 1);
        UINT32 p_key = CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));

        job->consumer_key_ = c_key;
        job->consumer_size_ = it->size_;
        job->producer_key_ = p_key;
        job->producer_size_ = it->size_;

        INT32 total_proc = job->proc_count_;
        JOBSTATUS * job_status = &jobs_[index++].status_;
        job_status->id_ = job->id_;
        job_status->total_proc_count_ = job->proc_count_;
        job_status->alive_proc_count_ = 0;
        job_status->dead_proc_count_ = 0;
        ProcStatus ** status_list = reinterpret_cast<ProcStatus **>(::malloc(sizeof(ProcStatus *) * total_proc));
        if (NULL == status_list) {
          NSF_PUBLIC_LOG_ERROR("ctrl monitor service load job failed msg: malloc array object of status failed");
          return Err::kERR_CTRL_LOAD_AP_MALLOC_OBJECT_FAILED;
        }
        for (INT32 i = 0; i < total_proc; ++i) {
          ProcStatus * status = reinterpret_cast<ProcStatus *>(::malloc(sizeof(ProcStatus)));
          if (NULL == status) {
            NSF_PUBLIC_LOG_ERROR("ctrl monitor service load job failed msg: malloc status object of status failed");
            return Err::kERR_CTRL_LOAD_AP_MALLOC_OBJECT_FAILED;
          } 
          ::memset(status, 0x00, sizeof(ProcStatus));
          status_list[i] = status;   
        }
        job_status->proc_status_ = status_list;

        ++it;
      }

      return 0;
    }

    const MonitorSvr::ProcessItem MonitorSvr::LoadToolItem(const conf & config, INT32 index) {
      ProcessItem item;
      CHAR key[1024] = {0};
      ::snprintf(key, sizeof(key), "ctrl.tools.items%d.groupid", index);
      item.id_ = config.GetINT32(key);
      ::snprintf(key, sizeof(key), "ctrl.tools.items%d.bin", index);
      item.bin_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "ctrl.tools.items%d.etc", index);
      item.etc_ = config.GetString(key);
      ::snprintf(key, sizeof(key), "ctrl.tools.items%d.count", index);
      item.count_ = config.GetINT32(key);
      ::snprintf(key, sizeof(key), "ctrl.tools.items%d.interval", index);
      item.active_interval_ = config.GetINT32(key);

      return item;
    }

    INT32 MonitorSvr::LoadTools(const ProcessList & conf) {
      INT32 size = conf.size();
      if (0 >= size) {
        return 0;
      }

      if (TOOL_MAX_COUNT < size) {
        return Err::kERR_CTRL_TOOLS_COUNT_INVALID;
      }

      ProcessList::const_iterator it = conf.begin();
      INT32 index = 0;
      while (conf.end() != it) {
        TOOLMETA * tool = &tools_[index].meta_;
        ::snprintf(tool->bin_file_, sizeof(tool->bin_file_), "%s", it->bin_.c_str());
        ::snprintf(tool->etc_file_, sizeof(tool->etc_file_), "%s", it->etc_.c_str());
        tool->id_ = it->id_;
        tool->heartbeat_ = it->active_interval_;
        tool->proc_count_ = 1;

        CHAR path[1024] = {0};
        ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*tool->id_);
        UINT32 c_key = CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));
        ::snprintf(path, sizeof(path), "%s_%d", Dir::GetBinDir().c_str(), 2*tool->id_ + 1);
        UINT32 p_key = CRC::CRC32(reinterpret_cast<UCHAR *>(path), ::strlen(path));

        tool->consumer_key_ = c_key;
        tool->consumer_size_ = it->size_;
        tool->producer_key_ = p_key;
        tool->producer_size_ = it->size_;

        INT32 total_proc = tool->proc_count_;
        TOOLSTATUS * tool_status = &tools_[index++].status_;
        tool_status->id_ = tool->id_;
        tool_status->total_proc_count_ = tool->proc_count_;
        tool_status->alive_proc_count_ = 0;
        tool_status->dead_proc_count_ = 0;
        ProcStatus ** status_list = reinterpret_cast<ProcStatus **>(::malloc(sizeof(ProcStatus *) * total_proc));
        if (NULL == status_list) {
          NSF_PUBLIC_LOG_ERROR("ctrl monitor service load tool failed msg: malloc array object of status failed");
          return Err::kERR_CTRL_LOAD_AP_MALLOC_OBJECT_FAILED;
        }
        for (INT32 i = 0; i < total_proc; ++i) {
          ProcStatus * status = reinterpret_cast<ProcStatus *>(::malloc(sizeof(ProcStatus)));
          if (NULL == status) {
            NSF_PUBLIC_LOG_ERROR("ctrl monitor service load tool failed msg: malloc status object of status failed");
            return Err::kERR_CTRL_LOAD_AP_MALLOC_OBJECT_FAILED;
          } 
          ::memset(status, 0x00, sizeof(ProcStatus));
          status_list[i] = status;   
        }
        tool_status->proc_status_ = status_list;

        ++it;
      }

      return 0;
    }

    INT32 MonitorSvr::CreateJobs(const ProcessList & conf) {
      INT32 result = LoadJobs(conf);
      if (0 != result) {
        return result;
      }

      return 0;
    }

    INT32 MonitorSvr::CreateTools(const ProcessList & conf) {
      INT32 result = LoadTools(conf);
      if (0 != result) {
        return result;
      }

      return 0;
    }

    INT32 MonitorSvr::Initial() {
      return DoCheck();
    }

    INT32 MonitorSvr::Running() {
      INT32 result = DoRecv();
      if (0 != result) {
        NSF_PUBLIC_LOG_ERROR("ctrl monitor service recv failed code:" << result);
      }

      result = DoCheck();
      if (0 != result) {
        NSF_PUBLIC_LOG_ERROR("ctrl monitor service check failed code:" << result);
      }

      result = DoUpdate();
      if (0 != result) {
        NSF_PUBLIC_LOG_ERROR("ctrl monitor service update failed code:" << result);
      }

      return 0;
    }

    INT32 MonitorSvr::DoCheck() {
      INT32 result = DoCheckAP();
      if (0 != result) {
        return result;
      }

      result = DoCheckJobs();
      if (0 != result) {
        return result;
      }

      result = DoCheckTools();
      if (0 != result) {
        return result;
      }

      return 0;
    }

    INT32 MonitorSvr::DoCheckAP() {
      INT32 alive = ap_.status_.alive_proc_count_;
      INT32 total = ap_.status_.total_proc_count_;
      if (0 >= total) {
        NSF_PUBLIC_LOG_WARN("ctrl monitor service ap processor count is {" << total << "}");
      }
      APMETA * ap_meta = &ap_.meta_;
      if (alive < total) {
        INT32 result = DoFork(ap_meta->bin_file_, ap_meta->etc_file_, total - alive, 0);
        if (0 != result) {
          NSF_PUBLIC_LOG_ERROR("fork ap failed total:" << total << " alive:" << alive << " code:" << result << " bin:" << ap_meta->bin_file_ << " etc:" << ap_meta->etc_file_);
          return result;
        }
      }

      return 0;
    }

    INT32 MonitorSvr::DoCheckJobs() {
      for (INT32 i = 0; i < JOB_MAX_COUNT; ++i) {
        JOB * job = &jobs_[i];
        // NSF_PUBLIC_LOG_DEBUG("jobs id:" << job->meta_.id_);
        if (0 < job->meta_.id_) {
          INT32 alive = job->status_.alive_proc_count_;
          INT32 total = job->status_.total_proc_count_;
          if (0 >= total) {
            NSF_PUBLIC_LOG_WARN("ctrl monitor service job processor count is {" << total << "}");
            continue;
          }
          JOBMETA * job_meta = &job->meta_;
          if (alive < total) {
            INT32 result = DoFork(job_meta->bin_file_, job_meta->etc_file_, total - alive, 0);
            if (0 != result) {
              NSF_PUBLIC_LOG_ERROR("fork job failed total:" << total << " alive:" << alive << " code:" << result << " bin:" << job_meta->bin_file_ << " etc:" << job_meta->etc_file_);
              return result;
            }
          }      
        }
      }

      return 0;
    }

    INT32 MonitorSvr::DoCheckTools() {
      for (INT32 i = 0; i < TOOL_MAX_COUNT; ++i) {
        TOOL * tool = &tools_[i];
        // NSF_PUBLIC_LOG_DEBUG("tools id:" << tool->meta_.id_);
        if (0 < tool->meta_.id_) {
          INT32 alive = tool->status_.alive_proc_count_;
          INT32 total = tool->status_.total_proc_count_;
          if (0 >= total) {
            NSF_PUBLIC_LOG_WARN("ctrl monitor service tool processor count is {" << total << "}");
            continue;
          }
          TOOLMETA * tool_meta = &tool->meta_;
          if (alive < total) {
            INT32 result = DoFork(tool_meta->bin_file_, tool_meta->etc_file_, total - alive, 0);
            if (0 != result) {
              NSF_PUBLIC_LOG_ERROR("fork tool failed total:" << total << " alive:" << alive << " code:" << result << " bin:" << tool_meta->bin_file_ << " etc:" << tool_meta->etc_file_);
              return result;
            }
          }      
        }
      }

      return 0;
    }

    INT32 MonitorSvr::DoRecv() {
      CHAR recv_buf[1024] = {0};
      while (TRUE) {
        INT32 result = mq_comm_.Recv(recv_buf, sizeof(recv_buf));
        if (0 >= result) {
          break;
        }

        if (sizeof(ProcHeartbeat) != result) {
          NSF_PUBLIC_LOG_WARN("monitor service receive invalid msg code:" << result);
          continue;
        }

        ProcHeartbeat * phb = reinterpret_cast<ProcHeartbeat *>(recv_buf);
        NSF_PUBLIC_LOG_DEBUG("monitor service receive heartbeat ................................");
        phb->Dump();
        if (phb->gid_ == ap_.meta_.id_) {
          result = DoRecvGroup(&ap_, phb);
          if (0 != result) {
            NSF_PUBLIC_LOG_WARN("monitor service do receive ap status failed code:" << result);
          }
        }

        for (INT32 i = 0; i < JOB_MAX_COUNT; ++i) {
          if (0 < jobs_[i].meta_.id_) {
            if (phb->gid_ == jobs_[i].meta_.id_) {
              result = DoRecvGroup(&jobs_[i], phb);
              if (0 != result) {
                NSF_PUBLIC_LOG_WARN("monitor service do receive job status failed code:" << result);
              }
            }            
          }
        }

        for (INT32 i = 0; i < TOOL_MAX_COUNT; ++i) {
          if (0 < tools_[i].meta_.id_) {
            if (phb->gid_ == tools_[i].meta_.id_) {
              result = DoRecvGroup(&tools_[i], phb);
              if (0 != result) {
                NSF_PUBLIC_LOG_WARN("monitor service do receive tool status failed code:" << result);
              }
            }            
          }
        }
      }

      return 0;
    }

    INT32 MonitorSvr::DoRecvGroup(Group * gp, ProcHeartbeat * phb) {
      if (NULL == gp || NULL == phb) {
        return Err::kERR_CTRL_DO_RECV_GROUP_OBJECT_INVALID;
      }

      INT32 pid = phb->pid_;

      GroupStatus * g_status = &gp->status_;
      ProcStatus ** p_status = g_status->proc_status_;
      INT32 index = 0;
      for (INT32 i = 0; i < g_status->total_proc_count_; ++i) {
        if (0 == p_status[i]->id_) {
          index = i;
          continue;
        }

        if (pid == p_status[i]->id_) {
          p_status[i]->timestamp_ = phb->timestamp_;
          return 0;
        }
      }

      if (g_status->alive_proc_count_ == g_status->total_proc_count_) {
        return Kill(pid);
      }

      p_status[index]->id_ = pid;
      p_status[index]->timestamp_ = phb->timestamp_;
      g_status->alive_proc_count_++;

      return 0;
    }

    INT32 MonitorSvr::DoFork(const string & bin, const string & etc, INT32 num, INT32 mask) {
      if (!Dir::FileExists(bin)) {
        return Err::kERR_CTRL_DO_FORK_BIN_FILE_NOT_EXISTS;
      }

      if (!Dir::FileExists(etc)) {
        return Err::kERR_CTRL_DO_FORK_ETC_FILE_NOT_EXISTS;
      }

      if (0 < mask) {
        if (0 > ::sched_setaffinity(::getpid(), sizeof(mask), (cpu_set_t *)&mask)) {
          return Err::kERR_CTRL_DO_FORK_SET_MASK_FAILED;
        }
      }

      CHAR buf[1024] = {0};
      ::snprintf(buf, sizeof(buf) - 1, "%s %s", bin.c_str(), etc.c_str());
      NSF_PUBLIC_LOG_DEBUG("command:" << buf);

      for (INT32 i = 0; i < num; ++i) {
        ::system(buf);
      }

      ::usleep(200000);

      return 0;
    }

    INT32 MonitorSvr::DoUpdate() {
      INT32 result = DoUpdateGroup(&ap_);
      if (0 != result) {
        NSF_PUBLIC_LOG_WARN("monitor service update ap status failed");
      }

      for (INT32 i = 0; i < JOB_MAX_COUNT; ++i) {
        if (0 >= jobs_[i].meta_.id_) {
          continue;
        }
        result = DoUpdateGroup(&jobs_[i]);
        if (0 != result) {
          NSF_PUBLIC_LOG_WARN("monitor service update jobs status failed");
        }
      }

      for (INT32 i = 0; i < TOOL_MAX_COUNT; ++i) {
        if (0 >= tools_[i].meta_.id_) {
          continue;
        }
        result = DoUpdateGroup(&tools_[i]);
        if (0 != result) {
          NSF_PUBLIC_LOG_WARN("monitor service update tools status failed");
        }
      }

      return 0;
    }

    INT32 MonitorSvr::DoUpdateGroup(Group * gp) {
      if (NULL == gp) {
        return Err::kERR_CTRL_DO_UPDATE_GROUP_OBJECT_INVALID;
      }

      UINT32 now = ::time(NULL);
      GroupStatus * g_status = &gp->status_;
      ProcStatus ** p_status = g_status->proc_status_;
      for (INT32 i = 0; i < g_status->total_proc_count_; ++i) {
        if (0 >= p_status[i]->id_) {
          continue;
        }

        if (gp->meta_.heartbeat_ <= now - p_status[i]->timestamp_) {
          p_status[i]->id_ = 0;
          p_status[i]->timestamp_ = 0;

          g_status->alive_proc_count_--;
          g_status->dead_proc_count_++;
        }
      }

      return 0;
    }

    INT32 MonitorSvr::Kill(INT32 pid) {
      INT32 result = ::kill(pid, SIGUSR1);
      if (0 != result) {
        NSF_PUBLIC_LOG_WARN("monitor service kill pid:" << pid << " failed");
        return Err::kERR_CTRL_DO_KILL_PROCESS_FAILED;
      }

      return 0;
    }

    VOID MonitorSvr::KillGroup(Group * gp) {
     if (NULL == gp) {
        return;
      }

      INT32 total = gp->status_.total_proc_count_;
      for (INT32 i = 0; i < total; ++i) {
        ProcStatus * status = gp->status_.proc_status_[i];
        if (status && (0 < status->id_)) {
          Kill(status->id_);
        }
      }
    }

    VOID MonitorSvr::KillAll() {
      KillGroup(&ap_);
      for (INT32 i = 0; i < JOB_MAX_COUNT; ++i) {
        if (0 >= jobs_[i].meta_.id_) {
          continue;
        }
        KillGroup(&jobs_[i]);
      }

      for (INT32 i = 0; i < TOOL_MAX_COUNT; ++i) {
        if (0 >= tools_[i].meta_.id_) {
          continue;
        }
        KillGroup(&tools_[i]);
      }
    }

    VOID MonitorSvr::Destroy() {
      mq_comm_.Destroy();
      DestroyGroup(&ap_);
      for (INT32 i = 0; i < JOB_MAX_COUNT; ++i) {
        if (0 >= jobs_[i].meta_.id_) {
          continue;
        }
        DestroyGroup(&jobs_[i]);
      }

      for (INT32 i = 0; i < TOOL_MAX_COUNT; ++i) {
        if (0 >= tools_[i].meta_.id_) {
          continue;
        }
        DestroyGroup(&tools_[i]);
      }
    }

    VOID MonitorSvr::DestroyGroup(Group * gp) {
      if (NULL == gp) {
        return;
      }

      INT32 total = gp->status_.total_proc_count_;
      for (INT32 i = 0; i < total; ++i) {
        ProcStatus * status = gp->status_.proc_status_[i];
        if (status) {
          ::free(status);
        }
      }
      if (gp->status_.proc_status_) {
        ::free(gp->status_.proc_status_);
      }
    }

    VOID MonitorSvr::Dump() {
      ap_.meta_.Dump();
      for (INT32 i = 0; i < JOB_MAX_COUNT; ++i) {
        if (0 <= jobs_[i].meta_.id_) {
          jobs_[i].meta_.Dump();
        }
      }
      for (INT32 i = 0; i < TOOL_MAX_COUNT; ++i) {
        if (0 <= tools_[i].meta_.id_) {
          tools_[i].meta_.Dump();
        }
      }
    }
  }  // namespace pub
}  // namespace nsf
