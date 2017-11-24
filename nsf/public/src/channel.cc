// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"
#include "nsf/public/inc/channel.h"
#include "nsf/public/inc/acce.h"

namespace nsf {
  namespace pub {
    INT32 Channel::CreatePipeList(const PipeList & pipe_list) {
      INT32 size = pipe_list.size();
      if (0 >= size) {
        return Err::kERR_CHANNEL_PIPE_LIST_EMPTY;
      }

      consumer_size_ = size;
      producer_size_ = size;

      NSF_PUBLIC_LOG_DEBUG("channel create pipe list size :" << size << " start ......");
      consumer_list_ = reinterpret_cast<Consumer **>(::malloc(size*sizeof(Consumer *)));
      if (NULL == consumer_list_) {
        return Err::kERR_CHANNEL_CONSUMER_LIST_CREATE_FAILED;
      }

      producer_list_ = reinterpret_cast<Producer **>(::malloc(size*sizeof(Producer *)));
      if (NULL == producer_list_) {
        return Err::kERR_CHANNEL_PRODUCER_LIST_CREATE_FAILED;
      }
      for (INT32 i = 0; i < size; ++i) {
        INT32 id = pipe_list[i].job_id_;
        INT32 shm_size = pipe_list[i].size_;
        INT32 consumer_key = pipe_list[i].consumer_key_;
        INT32 producer_key = pipe_list[i].producer_key_;
        INT32 lock = pipe_list[i].lock_;
        NSF_PUBLIC_LOG_DEBUG("channel create consumer pipe key :" << consumer_key);
        consumer_list_[i] = new Consumer();
        if (0 != consumer_list_[i]->CreatePipe(consumer_key, shm_size, lock, id)) {
          return Err::kERR_CHANNEL_CONSUMER_CREATE_FAILED;
        }

        NSF_PUBLIC_LOG_DEBUG("channel create producer pipe key :" << producer_key);
        producer_list_[i] = new Producer();
        if (0 != producer_list_[i]->CreatePipe(producer_key, shm_size, lock, id)) {
          return Err::kERR_CHANNEL_PRODUCER_CREATE_FAILED;
        }
      }

      NSF_PUBLIC_LOG_DEBUG("channel create pipe list success ");

      return 0;
    }

    VOID Channel::Destroy() {
      if (consumer_list_) {
        for (INT32 i = 0; i < consumer_size_; ++i) {
          delete consumer_list_[i];
        }

        ::free(consumer_list_);
        consumer_list_ = NULL;
      }
      if (producer_list_) {
        for (INT32 i = 0; i < producer_size_; ++i) {
          delete producer_list_[i];
        }

        ::free(producer_list_);
        producer_list_ = NULL;
      }
    }

    VOID Channel::Dump() {
      if (consumer_list_) {
        for (INT32 i = 0; i < consumer_size_; ++i) {
          consumer_list_[i]->Dump();
        }
      }
      if (producer_list_) {
        for (INT32 i = 0; i < producer_size_; ++i) {
          producer_list_[i]->Dump();
        }
      }
    }
  }  // namespace pub
}  // namespace nsf