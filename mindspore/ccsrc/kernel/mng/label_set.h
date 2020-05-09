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

#ifndef MINDSPORE_CCSRC_KERNEL_MNG_LABEL_SET_H
#define MINDSPORE_CCSRC_KERNEL_MNG_LABEL_SET_H

#include <memory>
#include <vector>
#include "kernel/mng/rt_kernel.h"
#include "kernel/mng/rt_kernel_info.h"

namespace mindspore {
namespace kernel {
class LabelSetKernel : public RtKernel {
 public:
  LabelSetKernel();
  ~LabelSetKernel() override;

  bool Init(const AnfNodePtr &anf_node) override;

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs, uintptr_t stream_ptr) override;
  std::vector<TaskInfoPtr> GenTask(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
                                   const std::vector<AddressPtr> &outputs, uint32_t stream_id) override;

 private:
  uint32_t label_;
};

MS_REG_RTKERNEL(labelset, LabelSetKernel);
}  // namespace kernel
}  // namespace mindspore

#endif  // MINDSPORE_CCSRC_KERNEL_MNG_LABEL_SET_H
