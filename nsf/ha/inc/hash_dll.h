// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_HA_INC_HASH_DLL_H_
#define NSF_HA_INC_HASH_DLL_H_

#include <stdlib.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/magic/inc/dll.h"

namespace nsf {
  namespace ha {
    using std::string;
    using lib::magic::DLL;

    typedef LONG (*_Hash)(const CHAR * id);
    typedef INT32 (*_DecodeUpPackage)(const CHAR * data, INT32 size, CHAR * id, INT32 * id_size, CHAR * seq, INT32 * seq_size);
    typedef INT32 (*_DecodeDownPackage)(const CHAR * data, INT32 size, CHAR * seq, INT32 * seq_size, INT32 * is_heart);
    typedef INT32 (*_EncodeHeart)(CHAR * data, INT32 * size);

    class HashDLL {
     public:
       HashDLL():hash_(NULL), decode_up_(NULL), decode_down_(NULL), encode_heart_(NULL) {}
       ~HashDLL() {}

     public:
       INT32 LoadingHashModule(const string & file);

     public:
       INT32 DecodeUp(const CHAR * data, INT32 size, CHAR * id, INT32 * id_size, CHAR * seq, INT32 * seq_size);
       INT32 DecodeDown(const CHAR * data, INT32 size, CHAR * seq, INT32 * seq_size, INT32 * is_heart);
       LONG Hash(const CHAR * id);
       INT32 EncodeHeat(CHAR * data, INT32 * size);

     protected:
       LONG DefHash(const CHAR * id);

     private:
       _Hash hash_;
       _DecodeUpPackage decode_up_;
       _DecodeDownPackage decode_down_;
       _EncodeHeart encode_heart_;
       DLL dll_;
    };
  }  // namespace ha
}  // namespace nsf

#endif  // NSF_HA_INC_HASH_DLL_H_
