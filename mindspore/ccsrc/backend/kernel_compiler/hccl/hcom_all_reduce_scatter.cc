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

#include "backend/kernel_compiler/hccl/hcom_all_reduce_scatter.h"
#include <memory>
#include "utils/ms_context.h"

namespace mindspore {
namespace kernel {
bool HcomAllReduceScatterKernel::Launch(const std::vector<AddressPtr> &inputs,
                                        const std::vector<AddressPtr> & /*workspace*/,
                                        const std::vector<AddressPtr> &outputs, void *stream_ptr) {
  auto context_ptr = MsContext::GetInstance();
  MS_EXCEPTION_IF_NULL(context_ptr);
  if (context_ptr->get_param<bool>(MS_CTX_ENABLE_TASK_SINK)) {
    return true;
  }
  if (inputs.empty() || outputs.empty() || hccl_data_type_list_.empty()) {
    MS_LOG(ERROR) << "ReduceScatter param is empty";
    return false;
  }
  const char *tag = "Hccl-ReduceScatter";
  hcclResult_t ret = hcom_reduce_scatter(tag, inputs[0]->addr, outputs[0]->addr, hccl_count_, hccl_data_type_list_[0],
                                         op_type_, nullptr, stream_ptr);
  if (ret != HCCL_SUCCESS) {
    MS_LOG(ERROR) << "HcomReduceScatterOp : hcom_reduce_scatter fail, return: " << static_cast<int>(ret);
    return false;
  }
  return true;
}
}  // namespace kernel
}  // namespace mindspore
