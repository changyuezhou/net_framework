// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_PUBLIC_INC_CHANNEL_H_
#define NSF_PUBLIC_INC_CHANNEL_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include "commlib/public/inc/type.h"
#include "nsf/public/inc/consumer.h"
#include "nsf/public/inc/producer.h"

namespace nsf {
  namespace pub {
    using std::string;
    using std::vector;
    using std::map;
    using std::make_pair;
    using std::pair;

    class Channel {
     public:
       typedef pair<Consumer **, INT32> ConsumerResult;
       typedef pair<Producer **, INT32> ProducerResult;

     public:
       typedef vector<PipeConfInfo> PipeList;

     public:
       Channel():consumer_list_(NULL), producer_list_(NULL), consumer_size_(0), producer_size_(0) {}
       ~Channel() { Destroy(); }

     public:
       INT32 CreatePipeList(const PipeList & pipe_list);

     public:
       ConsumerResult GetConsumerList() { return make_pair(consumer_list_, consumer_size_); }
       ProducerResult GetProducerList() { return make_pair(producer_list_, producer_size_); }

     public:
       VOID Destroy();
       VOID Dump();

     private:
       Consumer ** consumer_list_;
       Producer ** producer_list_;
       INT32 consumer_size_;
       INT32 producer_size_;
    };
  }  // namespace pub
}  // namespace nsf

#endif  // NSF_PUBLIC_INC_CHANNEL_H_