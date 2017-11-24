// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_HA_INC_UNIQ_MAP_H_
#define NSF_HA_INC_UNIQ_MAP_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include "commlib/public/inc/type.h"

namespace nsf {
  namespace ha {
    using std::string;
    using std::map;
    using std::pair;

    class UniqueMap {
     public:
       typedef map<string, string> UniqueItem;
       typedef pair<UniqueItem::iterator, BOOL> InsertResult;

     public:
       UniqueMap() { items_.clear(); }
       virtual ~UniqueMap() {}

     public:
       InsertResult Add(const string & key, const string & value);
       INT32 Add(const string & key, INT32 id, const string & uniq);
       const string & Get(const string & key);
       INT32 Get(const string & key, INT32 * id, CHAR * uniq, INT32 * uniq_size);
       BOOL IsExists(const string & key);

     protected:
       VOID Destroy() { items_.clear(); }

     private:
       UniqueItem items_;
    };

  }  // namespace ha
}  // namespace nsf

#endif  // NSF_HA_INC_UNIQ_MAP_H_
