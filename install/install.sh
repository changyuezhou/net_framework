#!/usr/bin/env bash

INSTALL_PATH=$0
cd $(dirname ${INSTALL_PATH})
INSTALL_PATH=`pwd`
NET_FRAMEWORK_DIR=$(dirname ${INSTALL_PATH})
BASE_DIR=$(dirname ${NET_FRAMEWORK_DIR})

function check_dir_exists() {
  DIR=$1
  if [ ! -x "$DIR" ]; then
    echo "-1"

    return
  fi

  echo "0"  
}

function check_home_log_dir() {
  HOME_DIR=$1
  LOG_DIR=$2

  RESULT=`check_dir_exists ${HOME_DIR}`
  if [ "$RESULT" == "0" ] ; then
    echo "[ERROR] HOME DIR:${HOME_DIR} is exists ..............................."

    return
  fi

  RESULT=`check_dir_exists ${LOG_DIR}`
  if [ "$RESULT" != "0" ] ; then
    mkdir -p "${LOG_DIR}"
  fi  

  RESULT=`check_dir_exists ${LOG_DIR}`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] LOG DIR:${LOG_DIR} is not exists ..............................."

    return
  fi

  RESULT=`create_nsf_log_dir  ${LOG_DIR}`
  RESULT=`create_plugin_log_dir  ${LOG_DIR}`

  RESULT=`check_dir_exists ${LOG_DIR}/nsf`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] LOG DIR:${LOG_DIR}/nsf is not exists ..............................."

    return
  fi 

  RESULT=`check_dir_exists ${LOG_DIR}/plugins`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] LOG DIR:${LOG_DIR}/plugins is not exists ..............................."

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

function check_plugin() {
  PLUGIN_LIB=$1
  PLUGIN_ETC=""

  PARAMS=$#
  if [ $PARAMS == 2 ] ; then
    let PLUGIN_ETC=$2
  fi

  RESULT=`check_file_exists ${PLUGIN_LIB}`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] PLUGIN LIB:${PLUGIN_LIB} is not exists ..............................."

    return
  fi

  if [ $PLUGIN_ETC != "" ] ; then
    RESULT=`check_file_exists ${PLUGIN_ETC}`
    if [ "$RESULT" != "0" ] ; then
      echo "[ERROR] PLUGIN ETC:${PLUGIN_ETC} is not exists ..............................."

      return
    fi
  fi

  echo "0" 
}

function create_nsf_log_dir() {
  LOG_DIR=$1

  mkdir -p "$LOG_DIR/nsf"
}

function create_plugin_log_dir() {
  LOG_DIR=$1

  mkdir -p "$LOG_DIR/plugins"
}

function create_dir_tree() {
  HOME_DIR=$1

  mkdir -p "${HOME_DIR}/bin"
  mkdir -p "${HOME_DIR}/etc"
  mkdir -p "${HOME_DIR}/plugins"
  mkdir -p "${HOME_DIR}/plugins/libs"
  mkdir -p "${HOME_DIR}/plugins/etc"
}

function copy_files_to_home_dir() {
  HOME_DIR=$1
  SOURCE_DIR=$2
  BUILD_DIR=$3

  cp "${SOURCE_DIR}/etc/ap.conf" "${HOME_DIR}/etc/"
  cp "${SOURCE_DIR}/etc/job.conf" "${HOME_DIR}/etc/"
  cp "${SOURCE_DIR}/etc/ctrl.conf" "${HOME_DIR}/etc/"

  cp "${SOURCE_DIR}/bin/service.sh" "${HOME_DIR}/bin/"

  sed -i "s#{HOME_DIR}#${HOME_DIR}#g" `grep '{HOME_DIR}' -rl ${HOME_DIR}/etc/`
  sed -i "s#{LOG_DIR}#${LOG_DIR}#g" `grep '{LOG_DIR}' -rl ${HOME_DIR}/etc/`
  cp "${BUILD_DIR}/nsf/ctrl/nsf_ctrl" "${HOME_DIR}/bin/"
  cp "${BUILD_DIR}/nsf/ap/nsf_ap" "${HOME_DIR}/bin/"
  cp "${BUILD_DIR}/nsf/job/nsf_job" "${HOME_DIR}/bin/"
}

function install_nsf() {
  HOME_DIR=$1
  LOG_DIR=$2

  RESULT=`check_home_log_dir ${HOME_DIR} ${LOG_DIR}`
  if [ "$RESULT" != "0" ] ; then
    echo "$RESULT"

    return
  fi

  RESULT=`create_dir_tree "${HOME_DIR}"`
  RESULT=`copy_files_to_home_dir "${HOME_DIR}" "${BASE_DIR}/net_framework" "${BASE_DIR}/build"`
  echo "0"
}

function copy_plugin_files() {
  HOME_DIR=$1
  PLUGIN_LIB=$2
  PLUGIN_ETC=$3

  cp "${PLUGIN_LIB}" "${HOME_DIR}/plugins/libs/"
  if [ "${PLUGIN_ETC}" != "" ] ; then
    cp "${PLUGIN_LIB}" "${HOME_DIR}/plugins/etc/"
  fi

  echo "0"
}

function install_plugin() {
  HOME_DIR=$1
  LOG_DIR=$2
  PLUGIN_LIB=$3
  PLUGIN_ETC=$4

  RESULT=`check_file_exists ${PLUGIN_LIB}`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] PLUGIN_LIB:${PLUGIN_LIB} is not exists ..............................."

    return
  fi

  if [ "${PLUGIN_ETC}" != "" ] ; then
    RESULT=`check_file_exists ${PLUGIN_ETC}`
    if [ "$RESULT" != "0" ] ; then
      echo "[ERROR] PLUGIN_ETC:${PLUGIN_ETC} is not exists ..............................."

      return
    fi
  fi

  RESULT=`check_dir_exists ${HOME_DIR}`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] HOME DIR:${HOME_DIR} is not exists ..............................."

    return
  fi

  RESULT=`check_dir_exists ${HOME_DIR}`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] HOME DIR:${HOME_DIR} is not exists ..............................."

    return
  fi

  RESULT=`check_dir_exists ${HOME_DIR}/plugins/libs`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] PLUGIN LIB DIR:${HOME_DIR}/plugins/libs is not exists ..............................."

    return
  fi

  RESULT=`check_dir_exists ${HOME_DIR}/plugins/etc`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] PLUGIN ETC DIR:${HOME_DIR}/plugins/etc is not exists ..............................."

    return
  fi

  RESULT=`check_dir_exists ${LOG_DIR}/plugins`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] PLUGINS LOG DIR:${LOG_DIR}/plugins is not exists ..............................."

    return
  fi

  RESULT=`copy_plugin_files ${HOME_DIR} ${PLUGIN_LIB} ${PLUGIN_ETC}`
  if [ "$RESULT" != "0" ] ; then
    echo "[ERROR] COPY PLUGIN LIB:${PLUGIN_LIB} ETC:${PLUGIN_ETC} FAILED ..............................."

    return
  fi

  PLUGIN_LIB_FILE_NAME="${PLUGIN_LIB##*/}"
  PLUGIN_ETC_FILE_NAME="${PLUGIN_ETC##*/}"
  sed -i "s#{PLUGIN_LIB}#${PLUGIN_LIB_FILE_NAME}#g" `grep '{PLUGIN_LIB}' -rl ${HOME_DIR}/etc/`
  sed -i "s#{PLUGIN_ETC}#${PLUGIN_ETC_FILE_NAME}#g" `grep '{PLUGIN_ETC}' -rl ${HOME_DIR}/etc/`

  echo "0"
}

PARAMS=$#
if [ $PARAMS != 3 ] && [ $PARAMS != 5 ] && [ $PARAMS != 4 ] ; then
    echo "Usage:[REQUIRED TYPE: NSF|PLUGIN|ALL] [REQUIRED HOME_DIR] [REQUIRED LOG_DIR] [OPTION: PLUGIN_LIB] [OPTION: PLUGIN_ETC]"
    exit
fi

if [ "$1" != "NSF" ] && [ "$1" != "PLUGIN" ] && [ "$1" != "ALL" ] ; then
    echo "[ERROR] TYPE must be NSF or PLUGIN or ALL ..............."
    echo "Usage:[REQUIRED TYPE: NSF|PLUGIN|ALL] [REQUIRED HOME_DIR] [REQUIRED LOG_DIR] [OPTION: PLUGIN_LIB] [OPTION: PLUGIN_ETC]"
    exit
fi

if [ $PARAMS == 3 ] && [ "$1" != "NSF" ] ; then
    echo "[ERROR] Need PLUGIN_LIB AND PLUGIN_ETC ..............."
    echo "Usage:[REQUIRED TYPE: NSF|PLUGIN|ALL] [REQUIRED HOME_DIR] [REQUIRED LOG_DIR] [OPTION: PLUGIN_LIB] [OPTION: PLUGIN_ETC]"
    exit
fi

if [ $PARAMS == 3 ] && [ "$1" == "NSF" ] ; then
    echo "INSTALL NSF START .........................................."
    HOME_DIR=$2
    LOG_DIR=$3
    RESULT=`install_nsf ${HOME_DIR} ${LOG_DIR}`
    if [ "$RESULT" != "0" ] ; then
      echo "$RESULT"
      exit
    fi

    echo "INSTALL NSF SUCCESS ........................................"
    exit
fi

if [ "$1" == "PLUGIN" ]; then
    echo "INSTALL PLUGIN START .........................................."
    HOME_DIR=$2
    LOG_DIR=$3
    PLUGIN_LIB=$4
    PLUGIN_ETC=""
    if [ $PARAMS == 5 ] ; then
      let PLUGIN_ETC=$5
    fi

    RESULT=`install_plugin ${HOME_DIR} ${LOG_DIR} ${PLUGIN_LIB} ${PLUGIN_ETC}`
    if [ "$RESULT" != "0" ] ; then
      echo "$RESULT"
      exit
    fi

    echo "INSTALL PLUGIN SUCCESS .........................................."
    exit      
fi

if [ "$1" == "ALL" ]; then
    echo "INSTALL NSF AND PLUGIN START .........................................."
    HOME_DIR=$2
    LOG_DIR=$3
    PLUGIN_LIB=$4
    PLUGIN_ETC=""
    if [ $PARAMS == 5 ] ; then
      let PLUGIN_ETC=$5
    fi

    RESULT=`install_nsf ${HOME_DIR} ${LOG_DIR}`
    if [ "$RESULT" != "0" ] ; then
      echo "$RESULT"
      exit
    fi

    RESULT=`install_plugin ${HOME_DIR} ${LOG_DIR} ${PLUGIN_LIB} ${PLUGIN_ETC}`
    if [ "$RESULT" != "0" ] ; then
      echo "$RESULT"
      exit
    fi

    echo "INSTALL NSF AND PLUGIN SUCCESS .........................................."
    exit      
fi

exit

