// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_PROC_REPORT_H_
#define NSF_PUBLIC_INC_PROC_REPORT_H_

#include <string>
#include <vector>
#include "commlib/public/inc/type.h"
#include "nsf/public/inc/log.h"

namespace nsf {
  namespace pub {
    using std::string;
    using std::vector;

    class ProcReport {
     public:
       static const INT32 MSG_MAX_LENGTH = 256;
       static const INT32 PATH_MAX_LENGTH = 256;
       static const INT32 CMD_DO_HEARTBEAT = 1;
       static const INT32 CMD_DO_QUIT = 2;
       static const INT32 CMD_DO_STATUS = 3;
       static const INT32 JOB_MAX_COUNT = 4;
       static const INT32 TOOL_MAX_COUNT = 4;

     public:
       struct ProcessItem {
         string bin_;
         string etc_;
         INT32  id_;
         INT32  size_;
         INT32  count_;
         INT32  active_interval_;

         ProcessItem() {
           bin_ = "";
           etc_ = "";
           id_ = -1;
           size_ = 0;
           count_ = 1;
           active_interval_ = 1;
         }

         const ProcessItem & operator=(const ProcessItem & left) {
           this->bin_ = left.bin_;
           this->etc_ = left.etc_;
           this->id_ = left.id_;
           this->size_ = left.size_;
           this->count_ = left.count_;
           this->active_interval_ = left.active_interval_;

           return *this;
         }

         VOID Dump() {
           NSF_PUBLIC_LOG_DEBUG("bin:" << bin_);
           NSF_PUBLIC_LOG_DEBUG("etc:" << etc_);
           NSF_PUBLIC_LOG_DEBUG("id:" << id_);
           NSF_PUBLIC_LOG_DEBUG("size:" << size_);
           NSF_PUBLIC_LOG_DEBUG("count:" << count_);
           NSF_PUBLIC_LOG_DEBUG("active_interval:" << active_interval_);
         }
       };

     public:
       typedef vector<ProcessItem> ProcessList;

     public:
       struct GroupMeta {
         INT32 id_;
         INT32 heartbeat_;
         INT32 proc_count_;
         UINT32 consumer_key_;
         INT32 consumer_size_;
         UINT32 producer_key_;
         INT32 producer_size_;
         INT32 affinity_;
         CHAR bin_file_[PATH_MAX_LENGTH];
         CHAR etc_file_[PATH_MAX_LENGTH];

         GroupMeta() {
           id_ = -1;
           heartbeat_ = 0;
           proc_count_ = 0;
           consumer_key_ = 0;
           consumer_size_ = 0;
           producer_key_ = 0;
           producer_size_ = 0;
           affinity_ = 0;
           ::strcpy(bin_file_, "");
           ::strcpy(etc_file_, "");
         }

         VOID Dump() {
           NSF_PUBLIC_LOG_DEBUG("id:" << id_);
           NSF_PUBLIC_LOG_DEBUG("heartbeat:" << heartbeat_);
           NSF_PUBLIC_LOG_DEBUG("proc_count:" << proc_count_);
           NSF_PUBLIC_LOG_DEBUG("consumer_key:" << consumer_key_);
           NSF_PUBLIC_LOG_DEBUG("consumer_size:" << consumer_size_);
           NSF_PUBLIC_LOG_DEBUG("producer_key:" << producer_key_);
           NSF_PUBLIC_LOG_DEBUG("producer_size:" << producer_size_);
           NSF_PUBLIC_LOG_DEBUG("affinity:" << affinity_);
           NSF_PUBLIC_LOG_DEBUG("bin_file:" << bin_file_);
           NSF_PUBLIC_LOG_DEBUG("etc_file:" << etc_file_);
         }
       };

       struct ProcStatus {
         INT32 id_;
         LONG timestamp_;

         ProcStatus() {
           id_ = -1;
           timestamp_ = 0;
         }

         VOID Dump() {
           NSF_PUBLIC_LOG_DEBUG("id:" << id_);
           NSF_PUBLIC_LOG_DEBUG("timestamp:" << timestamp_);
         }
       };

       struct GroupStatus {
         INT32 id_;
         INT32 total_proc_count_;
         INT32 alive_proc_count_;
         INT32 dead_proc_count_;
         ProcStatus ** proc_status_;

         GroupStatus() {
           id_ = -1;
           total_proc_count_ = 0;
           alive_proc_count_ = 0;
           dead_proc_count_ = 0;
           proc_status_ = NULL;
         }

         VOID Dump() {
           NSF_PUBLIC_LOG_DEBUG("******************************************");
           NSF_PUBLIC_LOG_DEBUG("id:" << id_);
           NSF_PUBLIC_LOG_DEBUG("total_proc_count:" << total_proc_count_);
           NSF_PUBLIC_LOG_DEBUG("alive_proc_count:" << alive_proc_count_);
           NSF_PUBLIC_LOG_DEBUG("dead_proc_count:" << dead_proc_count_);
           for (INT32 i = 0; i < total_proc_count_; ++i) {
             NSF_PUBLIC_LOG_DEBUG("idex:" << i);
             proc_status_[i]->Dump();
           }
         }
       };

       struct ProcHeartbeat {
         INT32 gid_;
         INT32 pid_;
         LONG timestamp_;
         ProcHeartbeat() {
           gid_ = -1;
           pid_ = -1;
           timestamp_ = 0;
         }

         VOID Dump() {
           NSF_PUBLIC_LOG_DEBUG("gid:" << gid_);
           NSF_PUBLIC_LOG_DEBUG("pid:" << pid_);
           NSF_PUBLIC_LOG_DEBUG("timestamp:" << timestamp_);
         }
       };

       struct MSG {
         long msg_type_;
         CHAR msg_text_[MSG_MAX_LENGTH];
       };

       struct Group {
         GroupMeta meta_;
         GroupStatus status_;
       };
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_PROC_REPORT_H_