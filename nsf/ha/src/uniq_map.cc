// Copyright (c) 2016 bruce qi. All rights reserved.

#include <string.h>
#include "nsf/ha/inc/uniq_map.h"
#include "nsf/ha/inc/log.h"
#include "commlib/magic/inc/str.h"

namespace nsf {
  namespace ha {
    using lib::magic::String;
    using lib::magic::StringSplit;

    UniqueMap::InsertResult UniqueMap::Add(const string & key, const string & value) {
      return items_.insert(pair<string, string>(key, value));;
    }

    INT32 UniqueMap::Add(const string & key, INT32 id, const string & uniq) {
      if (IsExists(key)) {
        return -1;
      }

      CHAR value[1024*2] = {0};
      ::snprintf(value, sizeof(value), "%d_%s", id, uniq.c_str());
      InsertResult result = Add(key, value);

      if (!result.second) {
        NSF_HA_LOG_WARN("unique map insert key:" << key << " value:" << value << " map size:" << items_.size() << " failed ...........................");

        return -1;
      }

      return 0;
    }

    const string & UniqueMap::Get(const string & key) {
      return items_[key];
    }

    INT32 UniqueMap::Get(const string & key, INT32 * id, CHAR * uniq, INT32 * uniq_size) {
      if (!IsExists(key)) {
        NSF_HA_LOG_WARN("unique map get key:" << key << " is not exists " << " map size:" << items_.size() << " ...........................");
        return -1;
      }

      const string & value = Get(key);
      StringSplit split(value, "_");
      if (2 != split.size()) {
        NSF_HA_LOG_WARN("unique map get key:" << key << " value:" << value << " is invalid " << " map size:" << items_.size() << " ...........................");
        return -1;
      }

      *id = ::atoi(split[0].c_str());
      *uniq_size = ::snprintf(uniq, *uniq_size, "%s", split[1].c_str());

      items_.erase(key);

      return 0;
    }

    BOOL UniqueMap::IsExists(const string & key) {
      if (0 >= items_.size()) {
        return FALSE;
      }

      if (items_.find(key) == items_.end()) {
        return FALSE;
      }

      return TRUE;
    }
  }  // namespace ha
}  // namespace nsf

