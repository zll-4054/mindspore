/**
 * Copyright 2019 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "minddata/dataset/util/services.h"

#include <limits.h>
#if !defined(_WIN32) && !defined(_WIN64) && !defined(__ANDROID__) && !defined(ANDROID)
#include <sys/syscall.h>
#else
#include <stdlib.h>
#endif
#include <unistd.h>
#include "./securec.h"
#include "minddata/dataset/util/circular_pool.h"
#include "minddata/dataset/util/random.h"
#include "minddata/dataset/util/task_manager.h"

namespace mindspore {
namespace dataset {
std::unique_ptr<Services> Services::instance_ = nullptr;
std::once_flag Services::init_instance_flag_;
std::set<std::string> Services::unique_id_list_ = {};
std::mutex Services::unique_id_mutex_;

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__ANDROID__) && !defined(ANDROID)
std::string Services::GetUserName() {
  char user[LOGIN_NAME_MAX];
  (void)getlogin_r(user, sizeof(user));
  return std::string(user);
}

std::string Services::GetHostName() {
  char host[LOGIN_NAME_MAX];
  (void)gethostname(host, sizeof(host));
  return std::string(host);
}

int Services::GetLWP() { return syscall(SYS_gettid); }
#endif

std::string Services::GetUniqueID() {
  const std::string kStr = "abcdefghijklmnopqrstuvwxyz0123456789";
  std::mt19937 gen = GetRandomDevice();
  std::uniform_int_distribution<uint32_t> dist(0, kStr.size() - 1);
  char buffer[UNIQUEID_LEN];
  {
    std::unique_lock<std::mutex> lock(unique_id_mutex_);
    while (true) {
      auto ret = memset_s(buffer, UNIQUEID_LEN, 0, UNIQUEID_LEN);
      if (ret != 0) {
        MS_LOG(ERROR) << "memset_s error, errorno(" << ret << ")";
        return std::string("");
      }
      for (int i = 0; i < UNIQUEID_LEN; i++) {
        buffer[i] = kStr[dist(gen)];
      }
      if (unique_id_list_.find(std::string(buffer, UNIQUEID_LEN)) != unique_id_list_.end()) {
        continue;
      }
      unique_id_list_.insert(std::string(buffer, UNIQUEID_LEN));
      break;
    }
  }
  return std::string(buffer, UNIQUEID_LEN);
}

Status Services::CreateAllInstances() {
  // First one is always the TaskManager
  RETURN_IF_NOT_OK(TaskManager::CreateInstance());
  TaskManager &tm = TaskManager::GetInstance();
  RETURN_IF_NOT_OK(tm.ServiceStart());
  return Status::OK();
}

Services::Services() : pool_(nullptr) {
  Status rc = CircularPool::CreateCircularPool(&pool_, -1, 16, true);  // each arena 16M
  if (rc.IsError()) {
    std::terminate();
  }
}

Services::~Services() noexcept {
  // Shutdown in reverse order.
  auto n = hook_.size();
  while (n > 0) {
    hook_.pop_back();
    n = hook_.size();
  }
}
}  // namespace dataset
}  // namespace mindspore