// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include <string.h>
#include "nsf/ha/inc/hash_dll.h"
#include "nsf/ha/inc/log.h"
#include "commlib/magic/inc/str.h"
#include "external/lib/inc/md5.h"

namespace nsf {
  namespace ha {
    using lib::magic::String;
    using ext::lib::MD5;

    INT32 HashDLL::LoadingHashModule(const string & file) {
      INT32 result = dll_.CreateDll(file);
      if (0 != result) {
        return result;
      }

      hash_ = reinterpret_cast<_Hash>(dll_.GetFunctionHandle("Hash"));
      decode_up_ = reinterpret_cast<_DecodeUpPackage>(dll_.GetFunctionHandle("DecodeUpPackage"));
      decode_down_ = reinterpret_cast<_DecodeDownPackage>(dll_.GetFunctionHandle("DecodeDownPackage"));
      encode_heart_ = reinterpret_cast<_EncodeHeart>(dll_.GetFunctionHandle("EncodeHeart"));
      if (NULL == encode_heart_ || NULL == decode_down_) {
        return -1;
      }

      return 0;
    }

    INT32 HashDLL::DecodeUp(const CHAR * data, INT32 size, CHAR * id, INT32 * id_size, CHAR * seq, INT32 * seq_size) {
      if (decode_up_) {
        return decode_up_(data, size, id, id_size, seq, seq_size);
      }

      return -1;
    }

    INT32 HashDLL::DecodeDown(const CHAR * data, INT32 size, CHAR * seq, INT32 * seq_size, INT32 * is_heart) {
      if (decode_down_) {
        return decode_down_(data, size, seq, seq_size, is_heart);
      }

      return -1;
    }

    LONG HashDLL::Hash(const CHAR * id) {
      if (hash_) {
        return hash_(id);
      }

      return DefHash(id);
    }

    LONG HashDLL::DefHash(const CHAR * id) {
      if (NULL == id || 0 >= String::Trim(id).length()) {
        return 0;
      }

      LONG hash = 0;
      MD5 md5(id);
      const string digest = md5.hex_digest();

      for(INT32 i = 0; i < 4; ++i)
      {
        hash += ((LONG)(digest.c_str()[i*4 + 3]&0xFF) << 24)
                | ((LONG)(digest.c_str()[i*4 + 2]&0xFF) << 16)
                | ((LONG)(digest.c_str()[i*4 + 1]&0xFF) <<  8)
                | ((LONG)(digest.c_str()[i*4 + 0]&0xFF));
      }

      return hash;
    }

    INT32 HashDLL::EncodeHeat(CHAR * data, INT32 * size) {
      if (encode_heart_) {
        return encode_heart_(data, size);
      }

      return (*size = 0);
    }
  }  // namespace ha
}  // namespace nsf
