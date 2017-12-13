// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_MONITOR_SVR_H_
#define NSF_PUBLIC_INC_MONITOR_SVR_H_

#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "nsf/public/inc/mq_comm.h"
#include "nsf/public/inc/proc_report.h"
#include "nsf/public/inc/err.h"

namespace nsf {
  namespace pub {
    using lib::etc::conf;

    class MonitorSvr {
     public:
       static const INT32 JOB_MAX_COUNT = ProcReport::JOB_MAX_COUNT;
       static const INT32 TOOL_MAX_COUNT = ProcReport::TOOL_MAX_COUNT;
       typedef ProcReport::Group Group;
       typedef ProcReport::GroupMeta GroupMeta;
       typedef ProcReport::GroupStatus GroupStatus;
       typedef ProcReport::Group AP;
       typedef ProcReport::GroupMeta APMETA;
       typedef ProcReport::GroupStatus APSTATUS;
       typedef ProcReport::Group JOB;
       typedef ProcReport::GroupMeta JOBMETA;
       typedef ProcReport::GroupStatus JOBSTATUS;
       typedef ProcReport::Group TOOL;
       typedef ProcReport::GroupMeta TOOLMETA;
       typedef ProcReport::GroupStatus TOOLSTATUS;
       typedef ProcReport::ProcessItem ProcessItem;
       typedef ProcReport::ProcessList ProcessList;
       typedef ProcReport::ProcHeartbeat ProcHeartbeat;
       typedef ProcReport::ProcStatus ProcStatus;

     public:
       MonitorSvr():initial_check_factor_(3) {}
       ~MonitorSvr() { Destroy(); }

     public:
       INT32 Create(const conf & config);
       INT32 Initial(INT32 initial_check_factor);
       INT32 Running();

     public:
       VOID Dump();

     public:
       VOID KillAll();
       VOID Destroy();

     protected:
       INT32 DoCheck();
       INT32 DoRecv();

     private:
       const ProcessItem LoadAPItem(const conf & config);
       const ProcessItem LoadJobItem(const conf & config, INT32 index);
       const ProcessItem LoadToolItem(const conf & config, INT32 index);
       INT32 LoadAP(const ProcessItem & conf);
       INT32 CreateAP(const ProcessItem & conf);
       INT32 LoadJobs(const ProcessList & conf);
       INT32 LoadTools(const ProcessList & conf);
       INT32 CreateJobs(const ProcessList & conf);
       INT32 CreateTools(const ProcessList & conf);

     private:
       INT32 DoFork(const string & bin, const string & etc, INT32 num, INT32 mask);

     private:
       INT32 DoCheckAP();
       INT32 DoCheckJobs();
       INT32 DoCheckTools();

     private:
       INT32 DoUpdate();
       INT32 DoUpdateGroup(Group * gp);

     private:
       INT32 Kill(INT32 pid);
       VOID KillGroup(Group * gp);
       VOID DestroyGroup(Group * gp);

     private:
       INT32 DoRecvGroup(Group * gp, ProcHeartbeat * phb);

     private:
       INT32 initial_check_factor_;
       MQComm mq_comm_;
       AP ap_;
       JOB jobs_[JOB_MAX_COUNT];
       TOOL tools_[TOOL_MAX_COUNT];
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_MONITOR_SVR_H_
