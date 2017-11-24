// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_HA_INC_AGENT_H_
#define NSF_HA_INC_AGENT_H_

#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include "commlib/public/inc/type.h"
#include "commlib/etc/inc/conf.h"
#include "nsf/ha/inc/uniq_map.h"
#include "nsf/ha/inc/config.h"
#include "nsf/ha/inc/hash_dll.h"
#include "nsf/public/inc/channel.h"
#include "nsf/public/inc/pipe_comm.h"

namespace nsf {
  namespace ha {
    using std::string;
    using std::vector;
    using lib::etc::conf;
    using lib::etc::ini;
    using nsf::pub::Channel;
    using nsf::pub::Consumer;
    using nsf::pub::Producer;
    using nsf::pub::PipeConfInfo;

    typedef pair<Consumer **, INT32> ConsumerResult;
    typedef pair<Producer **, INT32> ProducerResult;
    typedef vector<PipeConfInfo> PipeList;

    class Agent {
     public:
       Agent() {}
       virtual ~Agent() {}

     public:
       INT32 WaitForData(CHAR * in, INT32 * size, CHAR * id, INT32 * id_size);
       INT32 SendData(const CHAR * out, INT32 size);
       INT32 Initial(const string & file);
       INT32 Initial(Config & config);

     public:
       HashDLL * GetHashDLL() { return &hash_dll_; }

     private:
       HashDLL hash_dll_;
       UniqueMap uniq_map_;
       Channel channel_;
    };
  }  // namespace ha
}  // namespace nsf

#endif  // NSF_HA_INC_AGENT_H_
