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

#include "internal/src/kernel/fp32_grad/arithmetic_self_grad.h"
#include "internal/include/errorcode.h"
#include "internal/include/ms_tensor.h"
#include "utils/log_adapter.h"
#include "nnacl/fp32/arithmetic_self.h"
#include "nnacl/fp32/arithmetic.h"

int DoArithmeticGradSelf(TensorPtrVector in_tensors, TensorPtrVector out_tensors, Node *node,
                         mindspore::lite::Allocator *allocator) {
  size_t data_size = in_tensors[0]->ElementsNum();
  OpParameter *param = node->primitive_;
  float *dy_data = (float *)in_tensors[0]->data_;
  float *x_data = (float *)in_tensors[1]->data_;
  float *dx_data = (float *)(float *)out_tensors[0]->data_;
  int ret;
  if (param->type_ == KernelType::LogGrad) {
    ret = ElementDiv(dy_data, x_data, dx_data, data_size);
  } else if (param->type_ == KernelType::NegGrad) {
    ret = ElementNegative(dy_data, dx_data, data_size);
  } else {
    MS_LOG(ERROR) << "Unsupport kernel type: " << param->type_;
    return RET_PARAM_INVALID;
  }
  if (ret != NNACL_OK) {
    MS_LOG(ERROR) << "do arithmetic " << param->type_ << " fail!ret: " << ret;
    return RET_ERROR;
  }
  return RET_OK;
}
