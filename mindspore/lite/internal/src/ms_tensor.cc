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
#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include "internal/include/ms_tensor.h"
MSTensor *CreateTensor(TypeId data_type, const ShapeVector &shape) {
  MSTensor *tensor = new MSTensor();
  tensor->shape_ = shape;
  tensor->data_type_ = data_type;
  return tensor;
}
int MSTensor::ElementsNum() const { return std::accumulate(shape_.begin(), shape_.end(), 1LL, std::multiplies<int>()); }

size_t MSTensor::Size() const {
  size_t size = 0;
  switch (this->data_type_) {
    case kNumberTypeFloat64:
      size = sizeof(double);
      break;
    case kNumberTypeFloat:
    case kNumberTypeFloat32:
      size = sizeof(float);
      break;
    case kNumberTypeInt8:
      size = sizeof(int8_t);
      break;
    case kNumberTypeUInt8:
      size = sizeof(uint8_t);
      break;
    case kNumberTypeFloat16:
      size = sizeof(int16_t);
      break;
    case kNumberTypeInt16:
      size = sizeof(int16_t);
      break;
    case kNumberTypeInt32:
      size = sizeof(int32_t);
      break;
    case kNumberTypeInt64:
      size = sizeof(int64_t);
      break;
    case kNumberTypeUInt16:
      size = sizeof(uint16_t);
      break;
    case kNumberTypeUInt32:
      size = sizeof(uint32_t);
      break;
    case kNumberTypeUInt64:
      size = sizeof(uint64_t);
      break;
    case kNumberTypeBool:
      size = sizeof(bool);
      break;
    default:
      std::cout << "Not support the type: " << this->data_type_;
      return 0;
  }
  size *= (format_ == Format::Format_NC4HW4 || format_ == Format::Format_NHWC4) ? ElementsC4Num() : ElementsNum();

  return size;
}
int32_t MSTensor::Batch() const {
  if (this->shape_.size() != 4 && this->shape_.size() != 2) {
    std::cout << "Unsupported tensor shape: " << this->shape_.size();
    return -1;
  }
  switch (this->format_) {
    case Format::Format_NHWC:
    case Format::Format_NHWC4:
    case Format::Format_NCHW:
    case Format::Format_NC4HW4:
    case Format::Format_KCHW:
    case Format::Format_KHWC:
    case Format::Format_NC:
    case Format::Format_NC4:
      return this->shape_[0];
    case Format::Format_HWCK:
    case Format::Format_CHWK:
      return this->shape_[3];
    case Format::Format_HWKC:
      return this->shape_[2];
    case Format::Format_CKHW:
      return this->shape_[1];
    default:
      // std::cout << "Unsupported format: " << EnumNameFormat(this->format_);
      return -1;
  }
}

int32_t MSTensor::Channel() const {
  if (this->shape_.size() != 4 && this->shape_.size() != 2) {
    std::cout << "Unsupported tensor shape: " << this->shape_.size();
    return -1;
  }
  switch (this->format_) {
    case Format::Format_NCHW:
    case Format::Format_KCHW:
    case Format::Format_NC:
    case Format::Format_NC4:
      return this->shape_[1];
    case Format::Format_HWCK:
      return this->shape_[2];
    case Format::Format_HWKC:
    case Format::Format_NHWC:
    case Format::Format_NHWC4:
    case Format::Format_NC4HW4:
    case Format::Format_KHWC:
      return this->shape_[3];
    case Format::Format_CKHW:
    case Format::Format_CHWK:
      return this->shape_[0];
    default:
      return -1;
  }
}

int32_t MSTensor::Height() const {
  if (this->shape_.size() != 4 && this->shape_.size() != 2) {
    std::cout << "Unsupported tensor shape: " << this->shape_.size();
    return -1;
  }
  switch (this->format_) {
    case Format::Format_NCHW:
    case Format::Format_KCHW:
    case Format::Format_CKHW:
      return this->shape_[2];
    case Format::Format_NHWC:
    case Format::Format_NHWC4:
    case Format::Format_NC4HW4:
    case Format::Format_KHWC:
    case Format::Format_CHWK:
      return this->shape_[1];
    case Format::Format_HWCK:
    case Format::Format_HWKC:
    case Format::Format_HW:
    case Format::Format_HW4:
      return this->shape_[0];
    default:
      // std::cout << "Unsupported format: " << EnumNameFormat(this->format_);
      return -1;
  }
}

int32_t MSTensor::Width() const {
  if (this->shape_.size() != 4 && this->shape_.size() != 2) {
    std::cout << "Unsupported tensor shape: " << this->shape_.size();
    return -1;
  }
  switch (this->format_) {
    case Format::Format_NCHW:
    case Format::Format_KCHW:
    case Format::Format_CKHW:
      return this->shape_[3];
    case Format::Format_KHWC:
    case Format::Format_NHWC:
    case Format::Format_NHWC4:
    case Format::Format_NC4HW4:
    case Format::Format_CHWK:
      return this->shape_[2];
    case Format::Format_HWCK:
    case Format::Format_HWKC:
    case Format::Format_HW:
    case Format::Format_HW4:
      return this->shape_[1];
    default:
      return -1;
  }
}

int MSTensor::ElementsC4Num() const {
  int result = 0;
  if (this->shape_.size() == 4) {
    result = Batch() * Height() * Width() * ((Channel() + 3) / 4 * 4);
  } else if (this->shape_.size() == 2) {
    result = this->shape_[0] * ((this->shape_[1] + 3) / 4 * 4);
  }
  return result;
}
