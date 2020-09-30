/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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
#ifndef MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP16_STACK_FP16_H_
#define MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP16_STACK_FP16_H_

#include <vector>
#include "src/lite_kernel.h"
#include "src/runtime/kernel/arm/fp32/stack.h"

namespace mindspore::kernel {
class StackFp16CPUKernel : public StackCPUKernel {
 public:
  StackFp16CPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                     const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx,
                     const mindspore::lite::PrimitiveC *primitive)
      : StackCPUKernel(parameter, inputs, outputs, ctx, primitive) {}

  ~StackFp16CPUKernel() = default;

  int Init() override;
  int Run() override;

 private:
  void InitMallocFlags();
  int MallocAssignBuffer();
  void FreeBuffer();

 private:
  std::vector<bool> malloc_buffers_;
  std::vector<float16_t *> buffers_;
  float16_t *out_buffer_;
  bool malloc_out;
};
}  // namespace mindspore::kernel

#endif  // MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP16_STACK_FP16_H_