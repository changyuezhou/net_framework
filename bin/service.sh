#!/usr/bin/env bash

SHELL_PATH=$0
HOME_DIR=$(dirname $(dirname ${SHELL_PATH}))
INITIAL_FACTOR=50 # UNIT: 100 millisecond

function check_dir_exists() {
  DIR=$1
  if [ ! -x "$DIR" ]; then
    echo "-1"

    return
  fi

  echo "0"  
}

function check_file_exists() {
  FILE=$1

  if [ ! -f "$FILE" ]; then  
    echo "[ERROR] FILE:$FILE is not exists ............................"
　  return
  fi 

  echo "0" 
}

function check_tree_files () {
  RESULT=`check_file_exists ${HOME_DIR}/bin/nsf_ctrl`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] CTRL:${HOME_DIR}/bin/nsf_ctrl is not exists ..............................."

    return
  fi

  RESULT=`check_file_exists ${HOME_DIR}/bin/nsf_ap`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] AP:${HOME_DIR}/bin/nsf_ap is not exists ..............................."

    return
  fi

  RESULT=`check_file_exists ${HOME_DIR}/bin/nsf_job`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] JOB:${HOME_DIR}/bin/nsf_job is not exists ..............................."

    return
  fi


  RESULT=`check_file_exists ${HOME_DIR}/etc/ctrl.conf`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] CTRL ETC:${HOME_DIR}/etc/ctrl.conf is not exists ..............................."

    return
  fi

  RESULT=`check_file_exists ${HOME_DIR}/etc/ap.conf`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] AP ETC:${HOME_DIR}/etc/ap.conf is not exists ..............................."

    return
  fi

  RESULT=`check_file_exists ${HOME_DIR}/etc/job.conf`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] JOB ETC:${HOME_DIR}/etc/job.conf is not exists ..............................."

    return
  fi
 
  echo "0"
}

function check_plugin_lib() {
  PLUGIN_LIB=`cat "${HOME_DIR}/etc/job.conf" | grep "${HOME_DIR}/plugins/libs/"`
  PLUGIN_LIB=${PLUGIN_LIB#*\"}
  PLUGIN_LIB=${PLUGIN_LIB%\"*}
  
  RESULT=`check_file_exists ${PLUGIN_LIB}`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] PLUGIN LIB:${PLUGIN_LIB} is not exists ..............................."

    return
  fi

  RESULT=`ldd -r ${PLUGIN_LIB}`
  RESULT=`echo "${RESULT}" | grep "undefined symbol"`
  if [ "$RESULT" != "" ] ; then
    echo "[ERROR] PLUGIN LIB:${PLUGIN_LIB} IS INVALID PLZ RUN ldd -r ${PLUGIN_LIB}"
    return
  fi

  PLUGIN_ETC=`cat "${HOME_DIR}/etc/job.conf" | grep "${HOME_DIR}/plugins/etc/"`
  PLUGIN_ETC=${PLUGIN_ETC#*\"}
  PLUGIN_ETC=${PLUGIN_ETC%\"*}

  ETC_FILE=${PLUGIN_ETC##*/}

  if [ "${ETC_FILE}" != "" ] ; then
    RESULT=`check_file_exists ${PLUGIN_ETC}`
    if [ "$RESULT" != "0" ] ; then
      echo "[ERROR] PLUGIN ETC:${PLUGIN_ETC} is not exists ..............................."

      return
    fi
  fi

  echo "0" 
}

PARAMS=$#
if [ $PARAMS != 1 ] ; then
    echo "Usage: [REQUIRED COMMAND: START|STOP]"
    exit
fi

if [ "$1" != "START" ] && [ "$1" != "STOP" ] ; then
    echo "[ERROR] COMMAND must be START or STOP ..............."
    echo "Usage: [REQUIRED COMMAND: START|STOP]"
    exit
fi

if [ "$1" == "START" ] ; then
  echo "NSF START RUNNING .........................................."
  RESULT=`check_tree_files`
  if [ "${RESULT}" != "0" ] ; then
    echo "$RESULT"
    exit
  fi
  echo "NSF RUNNING CHECK FILES SUCCESS ............................"

  RESULT=`check_plugin_lib`
  if [ "${RESULT}" != "0" ] ; then
    echo "$RESULT"
    exit
  fi
  echo "NSF RUNNING CHECK PLUGIN LIB SUCCESS ............................"

  ${HOME_DIR}/bin/nsf_ctrl ${HOME_DIR}/etc/ctrl.conf ${INITIAL_FACTOR}

  echo "NSF RUNNING SUCCESS .........................................."
fi

if [ "$1" == "STOP" ] ; then
  echo "NSF STOP .........................................."

  SERVER_NAME=${HOME_DIR}/bin/nsf_ctrl
  PID=`ps -ef | grep $SERVER_NAME | grep -v grep | tr -s ' ' | cut -d' ' -f2`
  kill -10 ${PID}

  echo "NSF STOP SUCCESS .........................................."
fi

exit

