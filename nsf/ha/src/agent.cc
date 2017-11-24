// Copyright (c) 2016 bruce qi. All rights reserved.

#include <string.h>
#include "commlib/magic/inc/str.h"
#include "nsf/ha/inc/agent.h"
#include "nsf/ha/inc/log.h"

namespace nsf {
  namespace ha {
    using lib::magic::String;

    INT32 Agent::Initial(const string & file) {
      Config config;
      if (0 != config.Loading(file)) {
        return -1;
      }

      return Initial(config);
    }

    INT32 Agent::Initial(Config & config) {
      PipeList list;
      list.push_back(config.GetPipeInfo());
      if (0 != channel_.CreatePipeList(list)) {
        return -1;
      }

      if (0 != hash_dll_.LoadingHashModule(config.GetHashFile())) {
        NSF_HA_LOG_ERROR("nsf ha agent load hash file failed...........................");
        return -1;
      }

      return 0;
    }

    INT32 Agent::WaitForData(CHAR * in, INT32 * size, CHAR * id, INT32 * id_size) {
      ConsumerResult c_result = channel_.GetConsumerList();
      Consumer * consumer = c_result.first[0];

      CHAR uniq[1024] = {0};
      INT32 ext = 0;
      CHAR seq[1024] = {0};
      INT32 seq_size = sizeof(seq);
      INT32 result = consumer->Consuming(in, size, &ext, uniq, sizeof(uniq));
      if (0 != result) {
        return result;
      }

      if (0 != hash_dll_.DecodeUp(in, *size, id, id_size, seq, &seq_size)) {
        NSF_HA_LOG_WARN("nsf ha agent receive data failed decode up size:" << *size << " ...........................");

        return -1;
      }

      if (0 >= String::Trim(seq).length()) {
        NSF_HA_LOG_WARN("nsf ha agent receive data failed seq is empty size:" << *size << " ...........................");

        return -1;
      }

      NSF_HA_LOG_DEBUG("nsf ha receive data success seq:" << seq << " id:" << id << " ...........................");

      if (0 != uniq_map_.Add(seq, ext, uniq)) {
        NSF_HA_LOG_WARN("nsf ha add to unique map seq:" << seq << " is duplicate ...........................");

        ProducerResult p_result = channel_.GetProducerList();
        Producer * producer = p_result.first[0];

        if (0 != producer->Producing("message sequence is duplicate\r\n", ::strlen("message sequence is duplicate\r\n"), ext, uniq)) {
          NSF_HA_LOG_WARN("nsf ha agent send data failed producing...........................");
        }
      }

      return 0;
    }

    INT32 Agent::SendData(const CHAR * out, INT32 size) {
      ProducerResult p_result = channel_.GetProducerList();
      Producer * producer = p_result.first[0];
      CHAR uniq[1024] = {0};
      INT32 uniq_size = sizeof(uniq);
      INT32 ext = 0;
      CHAR seq[1024] = {0};
      INT32 seq_size = sizeof(seq);
      INT32 is_heart = 0;

      if (0 != hash_dll_.DecodeDown(out, size, seq, &seq_size, &is_heart)) {
        NSF_HA_LOG_WARN("nsf ha agent send data failed decode down size:" << size << " ...........................");

        return -1;
      }

      if (0 < is_heart) {
        return 0;
      }

      if (0 >= seq_size || 0 >= String::Trim(seq).length()) {
        NSF_HA_LOG_WARN("nsf ha agent send data failed seq not found in down data size:" << size << " ...........................");

        return -1;
      }

      if (0 != uniq_map_.Get(seq, &ext, uniq, &uniq_size)) {
        NSF_HA_LOG_WARN("nsf ha agent send data failed seq:" << seq << " not found in map ...........................");

        return -1;
      }

      NSF_HA_LOG_DEBUG("nsf ha send data success seq:" << seq << " ...........................");

      if (0 != producer->Producing(out, size, ext, uniq)) {
        NSF_HA_LOG_WARN("nsf ha agent send data failed producing ...........................");

        return -1;
      }

      return 0;
    }
  }  // namespace ha
}  // namespace nsf

