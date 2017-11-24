// Copyright (c) 2013 zhou chang yue. All rights reserved.

#include "commlib/magic/inc/str.h"
#include "nsf/public/inc/err.h"
#include "nsf/public/inc/log.h"
#include "nsf/public/inc/pipe_comm.h"
#include "nsf/public/inc/acce.h"

namespace nsf {
  namespace  pub {
    using lib::magic::String;

    INT32 PipeComm::Create(INT32 key, INT32 size, INT32 id) {
      BOOL need_clear = FALSE;
      INT32 total_size = sizeof(PipeStatInfo) + sizeof(PipeRealTimeInfo) + size;
      INT32 shared_mem_id = 0;
      if (-1 != (shared_mem_id = ::shmget(key, total_size, 0))) {
        ptr_ = reinterpret_cast<CHAR *>(::shmat(shared_mem_id, NULL, 0));
      } else {
        shared_mem_id = ::shmget(key, total_size, IPC_CREAT|SHM_R|SHM_W|SHM_R>>3|SHM_R>>6);
        if (-1 == shared_mem_id) {
          return Err::kERR_PIPE_COMM_SHARED_MEMORY_GET_FAILED;
        }
        ptr_ = reinterpret_cast<CHAR *>(::shmat(shared_mem_id, NULL, 0));
        need_clear = TRUE;
      }
      if (ptr_ == MAP_FAILED) {
          NSF_PUBLIC_LOG_ERROR("shared memory at failed key:" << key << " size:" << size);
          return Err::kERR_PIPE_COMM_SHARED_MEMORY_AT_FAILED;
      }
      st_info_ = reinterpret_cast<PipeStatInfo *>(ptr_);
      rt_info_ = reinterpret_cast<PipeRealTimeInfo *>(ptr_ + sizeof(PipeStatInfo));
      data_ = ptr_ + sizeof(PipeStatInfo) + sizeof(PipeRealTimeInfo);
      if (need_clear) {
        rt_info_->total_len_ = size;
        rt_info_->w_ = 0;
        rt_info_->r_ = 0;
        ::memset(st_info_, 0x00, sizeof(PipeStatInfo));
      }

      key_ = key;

      return 0;
    }

    INT32 PipeComm::Enqueue(const CHAR * data, INT32 size, INT32 ext, const CHAR * uniq) {
      if (unlikely(NULL == rt_info_) || unlikely(NULL == st_info_)) {
        return Err::kERR_PIPE_COMM_RT_INFO_INVALID;
      }

      if (unlikely(IsFull())) {
        return Err::kERR_PIPE_COMM_BUFFER_FULL;
      }

      INT32 uniq_size = ::strlen(uniq);
      if (unlikely(UNIQ_LENGTH < uniq_size)) {
        return Err::kERR_PIPE_COMM_UNIQ_LENGTH_LARGER;
      }

      INT32 magic_num = MAGIC_NUM;

      INT32 w = rt_info_->w_;
      INT32 r = rt_info_->r_;
      INT32 total_len = rt_info_->total_len_;
      INT32 free_len = 0;
      if (w == r) {
        free_len = rt_info_->total_len_;
      } else {
        free_len = (total_len + r - w)%total_len;
      }
      INT32 data_total_size = size + DATA_INFO_LENGTH;

      if (unlikely(data_total_size > free_len)) {
        return Err::kERR_PIPE_COMM_BUFFER_FULL;
      }

      CHAR head[HEAD_LENGTH] = {0};
      ::memcpy(head, &magic_num, sizeof(magic_num));
      ::memcpy(head + TAG_LENGTH, &data_total_size, sizeof(data_total_size));
      ::memcpy(head + TAG_LENGTH + DATA_LENGTH, &ext, sizeof(ext));
      ::memcpy(head + TAG_LENGTH + DATA_LENGTH + EXT_LENGTH, uniq, uniq_size);

      INT32 direct_free = total_len - w;
      if (likely(direct_free >= data_total_size)) {
        ::memcpy(data_ + w, head, HEAD_LENGTH);
        ::memcpy(data_ + w + HEAD_LENGTH, data, size);
        ::memcpy(data_ + w + HEAD_LENGTH + size, &magic_num, sizeof(magic_num));
      } else if (direct_free > HEAD_LENGTH && direct_free < data_total_size) {
        ::memcpy(data_ + w, head, HEAD_LENGTH);
        INT32 rest_length = direct_free - HEAD_LENGTH;
        if (rest_length > size) {
          ::memcpy(data_ + w + HEAD_LENGTH, data, size);

          INT32 tail_len = rest_length - size;
          CHAR tail[sizeof(magic_num)] = {0};
          ::memcpy(tail, &magic_num, sizeof(magic_num));

          ::memcpy(data_ + w + HEAD_LENGTH + size, tail, tail_len);
          ::memcpy(data_, tail + tail_len, sizeof(magic_num) - tail_len);
        } else if (size == rest_length) {
          ::memcpy(data_ + w + HEAD_LENGTH, data, size);
          ::memcpy(data_, &magic_num, sizeof(magic_num));
        } else {
          ::memcpy(data_ + w + HEAD_LENGTH, data, rest_length);
          INT32 head_length = size - rest_length;
          ::memcpy(data_, data + rest_length, head_length);
          ::memcpy(data_ + head_length, &magic_num, sizeof(magic_num));
        }
      } else {
        ::memcpy(data_ + w, head, direct_free);
        INT32 head_length = HEAD_LENGTH - direct_free;
        ::memcpy(data_, head + direct_free, head_length);
        ::memcpy(data_ + head_length, data, size);
        ::memcpy(data_ + head_length + size, &magic_num, sizeof(magic_num));
      }

      rt_info_->w_ = (w + data_total_size)%total_len;

      atomic_add(1, &(st_info_->total_msg_count_));

      NSF_PUBLIC_LOG_DEBUG("pipe comm enqueue size:" << size << " ext:" << ext << " w: " << rt_info_->w_ << " r:" << rt_info_->r_);

      return 0;
    }

    INT32 PipeComm::Dequeue(CHAR * data, INT32 * size, INT32 * ext, CHAR * uniq, INT32 uniq_size) {
      if (unlikely(NULL == rt_info_) || unlikely(NULL == st_info_)) {
        return Err::kERR_PIPE_COMM_RT_INFO_INVALID;
      }

      if (unlikely(uniq_size <= UNIQ_LENGTH)) {
        return Err::kERR_PIPE_COMM_UNIQ_LENGTH_LARGER;
      }

      INT32 w = rt_info_->w_;
      INT32 r = rt_info_->r_;

      if (unlikely(w == r)) {
        return Err::kERR_PIPE_COMM_BUFFER_EMPTY;
      }

      INT32 total_len = rt_info_->total_len_;
      INT32 tail_len = total_len - r;
      CHAR head[HEAD_LENGTH] = {0};
      if (unlikely(tail_len < HEAD_LENGTH)) {
        ::memcpy(head, data_ + r, tail_len);
        ::memcpy(head + tail_len, data_, HEAD_LENGTH - tail_len);
      } else {
        ::memcpy(head, data_ + r, HEAD_LENGTH);
      }
      r = (r + HEAD_LENGTH)%total_len;

      INT32 tag = 0;
      ::memcpy(&tag, head, sizeof(tag));
      if (MAGIC_NUM != tag) {
        NSF_PUBLIC_LOG_ERROR("pipe dequeue head tag is invalid " << tag);
        Clear();
        return Err::kERR_PIPE_COMM_DATA_TAG_INVALID;
      }

      INT32 total_data_size = 0;
      ::memcpy(&total_data_size, head + TAG_LENGTH, sizeof(total_data_size));
      ::memcpy(ext, head + TAG_LENGTH + DATA_LENGTH, EXT_LENGTH);
      ::memcpy(uniq, head + TAG_LENGTH + DATA_LENGTH + EXT_LENGTH, UNIQ_LENGTH);
      INT32 data_len = total_data_size - DATA_INFO_LENGTH;
      if (data_len > *size) {
        NSF_PUBLIC_LOG_ERROR("pipe dequeue data len:" << data_len << " buffer size:" << *size);
        return Err::kERR_CONSUMER_PIPE_BUFFER_TOO_SMALLER;
      }

      tail_len = total_len - r;
      if (unlikely(tail_len < data_len)) {
        ::memcpy(data, data_ + r, tail_len);
        ::memcpy(data + tail_len, data_, data_len - tail_len);
      } else {
        ::memcpy(data, data_ + r, data_len);
      }
      r = (r + data_len)%total_len;

      tail_len = total_len - r;
      CHAR tail_arr[TAG_LENGTH] = {0};
      if (unlikely(tail_len < TAG_LENGTH)) {
        ::memcpy(tail_arr, data_ + r, tail_len);
        ::memcpy(tail_arr + tail_len, data_, TAG_LENGTH - tail_len);
      } else {
        ::memcpy(tail_arr, data_ + r, TAG_LENGTH);
      }
      r = (r + TAG_LENGTH)%total_len;
      ::memcpy(&tag, tail_arr, TAG_LENGTH);
      if (unlikely(MAGIC_NUM != tag)) {
        NSF_PUBLIC_LOG_ERROR("pipe dequeue tail tag is invalid " << tag << " data len:" << data_len << " data total len:" << total_data_size);
        Clear();
        return Err::kERR_PIPE_COMM_DATA_TAG_INVALID;
      }

      *size = data_len;

      rt_info_->r_ = r;

      atomic_add(1, &(st_info_->processed_msg_count_));

      NSF_PUBLIC_LOG_DEBUG("pipe comm dequeue size:" << *size << " ext:" << *ext << " r:" << rt_info_->r_ << " w:" << rt_info_->w_);

      return 0;
    }

    VOID PipeComm::Clear() {
      if (NULL == rt_info_ || NULL == st_info_) {
        return;
      }

      rt_info_->w_ = rt_info_->r_ = 0;
    }

    BOOL PipeComm::IsEmpty() {
      return (rt_info_->r_ == rt_info_->w_);
    }

    BOOL PipeComm::IsFull() {
      return ((rt_info_->w_ + 1)%rt_info_->total_len_ == rt_info_->r_);
    }

    VOID PipeComm::Dump() {
      NSF_PUBLIC_LOG_WARN("pipe key:" << key_);
      NSF_PUBLIC_LOG_WARN("pipe total length:" << rt_info_->total_len_);
      NSF_PUBLIC_LOG_WARN("pipe r:" << rt_info_->r_);
      NSF_PUBLIC_LOG_WARN("pipe w:" << rt_info_->w_);

      NSF_PUBLIC_LOG_WARN("pipe total msg:" << st_info_->total_msg_count_.counter);
      NSF_PUBLIC_LOG_WARN("pipe processed msg:" << st_info_->processed_msg_count_.counter);
    }
  }  // namespace pub
}  // namespace nsf