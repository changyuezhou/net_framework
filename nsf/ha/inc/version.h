// Copyright (c) 2013 zhou chang yue. All rights reserved.

#ifndef NSF_HA_INC_VERSION_H_
#define NSF_HA_INC_VERSION_H_

#include <stdlib.h>
#include <string>
#include "commlib/public/inc/type.h"
#include "commlib/magic/inc/str.h"

#define BUILD_MONTH_IS_JAN(x) (x[0] == 'J' && x[1] == 'a' && x[2] == 'n')
#define BUILD_MONTH_IS_FEB(x) (x[0] == 'F')
#define BUILD_MONTH_IS_MAR(x) (x[0] == 'M' && x[1] == 'a' && x[2] == 'r')
#define BUILD_MONTH_IS_APR(x) (x[0] == 'A' && x[1] == 'p')
#define BUILD_MONTH_IS_MAY(x) (x[0] == 'M' && x[1] == 'a' && x[2] == 'y')
#define BUILD_MONTH_IS_JUN(x) (x[0] == 'J' && x[1] == 'u' && x[2] == 'n')
#define BUILD_MONTH_IS_JUL(x) (x[0] == 'J' && x[1] == 'u' && x[2] == 'l')
#define BUILD_MONTH_IS_AUG(x) (x[0] == 'A' && x[1] == 'u')
#define BUILD_MONTH_IS_SEP(x) (x[0] == 'S')
#define BUILD_MONTH_IS_OCT(x) (x[0] == 'O')
#define BUILD_MONTH_IS_NOV(x) (x[0] == 'N')
#define BUILD_MONTH_IS_DEC(x) (x[0] == 'D')

namespace nsf {
  namespace ha {
    using std::string;
    using lib::magic::StringSplit;

    class Version {
     public:
       static const INT32 kMAJOR_VERSION = 1;
       static const INT32 kMINOR_VERSION = 0;
       static const INT32 kPATCH_VERSION = 0;
       static const CHAR * kDATE_VERSION;
       static const CHAR * kTIME_VERSION;

     public:
       static const string GetMonth(const string & mon) {
         if (BUILD_MONTH_IS_JAN(mon.c_str())) {
           return "01";
         } else if (BUILD_MONTH_IS_FEB(mon.c_str())) {
           return "02";
         } else if (BUILD_MONTH_IS_MAR(mon.c_str())) {
           return "03";
         } else if (BUILD_MONTH_IS_APR(mon.c_str())) {
           return "04";
         } else if (BUILD_MONTH_IS_MAY(mon.c_str())) {
           return "05";
         } else if (BUILD_MONTH_IS_JUN(mon.c_str())) {
           return "06";
         } else if (BUILD_MONTH_IS_JUL(mon.c_str())) {
           return "07";
         } else if (BUILD_MONTH_IS_AUG(mon.c_str())) {
           return "08";
         } else if (BUILD_MONTH_IS_SEP(mon.c_str())) {
           return "09";
         } else if (BUILD_MONTH_IS_OCT(mon.c_str())) {
           return "10";
         } else if (BUILD_MONTH_IS_NOV(mon.c_str())) {
           return "11";
         } else if (BUILD_MONTH_IS_DEC(mon.c_str())) {
           return "12";
         }

         return "";
       }

       static const string GetCurrent() {
         CHAR date[32] = {0};
         StringSplit date_split(kDATE_VERSION, " ");
         if (3 == date_split.TokenSize()) {
           ::snprintf(date, sizeof(date), "%s%s%s", date_split.GetToken(2).c_str(), GetMonth(date_split.GetToken(0).c_str()).c_str(), date_split.GetToken(1).c_str());
         }

         CHAR time[32] = {0};
         StringSplit time_split(kTIME_VERSION, ":");
         if (3 == time_split.TokenSize()) {
           ::snprintf(time, sizeof(time), "%s%s%s", time_split.GetToken(0).c_str(), date_split.GetToken(1).c_str(), time_split.GetToken(2).c_str());
         }
         CHAR version[1024] = {0};
         ::snprintf(version, sizeof(version), "%02d.%02d.%02d.%s%s",
                                              kMAJOR_VERSION,
                                              kMINOR_VERSION,
                                              kPATCH_VERSION,
                                              date,
                                              time);

         return version;
       }
    };
  }  // namespace ha
}  // namespace nsf

#endif  // NSF_HA_INC_VERSION_H_