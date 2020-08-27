/**
 * Copyright 2019-2020 Huawei Technologies Co., Ltd
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

#ifndef LITE_MINDSPORE_LITE_C_OPS_BATCH_TO_SPACE_H_
#define LITE_MINDSPORE_LITE_C_OPS_BATCH_TO_SPACE_H_

#include <vector>
#include <set>
#include <cmath>
#include <memory>
#include "ir/dtype/type_id.h"
#include "src/ops/primitive_c.h"

namespace mindspore {
namespace lite {
class BatchToSpace : public PrimitiveC {
 public:
#ifdef PRIMITIVE_WRITEABLE
  MS_DECLARE_PARENT(BatchToSpace, PrimitiveC);
  BatchToSpace() = default;
  explicit BatchToSpace(schema::PrimitiveT *primitive) : PrimitiveC(primitive) {}
  void SetBlockShape(const std::vector<int> &block_shape);
  void SetCrops(const std::vector<int> &crops);
#else
  explicit BatchToSpace(schema::Primitive *primitive) : PrimitiveC(primitive) {}

  schema::Primitive *Init(schema::Primitive *primitive) {
    flatbuffers::FlatBufferBuilder fbb(1024);

    auto attr = primitive->value_as_BatchToSpace();
    MS_ASSERT(attr != nullptr);

    auto blockShape = std::make_unique<std::vector<int32_t>>();
    for (int i = 0; i < static_cast<int>(attr->blockShape()->size()); i++) {
      blockShape->push_back(attr->blockShape()->data()[i]);
    }
    auto crops = std::make_unique<std::vector<int32_t>>();
    for (int i = 0; i < static_cast<int>(attr->crops()->size()); i++) {
      crops->push_back(attr->crops()->data()[i]);
    }

    auto val_offset = schema::CreateBatchToSpaceDirect(fbb, blockShape.release(), crops.release());
    auto prim_offset = schema::CreatePrimitive(fbb, schema::PrimitiveType_BatchToSpace, val_offset.o);
    fbb.Finish(prim_offset);

    auto buf = fbb.GetBufferPointer();
    MS_ASSERT(buf != nullptr);
    auto buf_bak = new char[fbb.GetSize()];
    memcpy(buf_bak, buf, fbb.GetSize());

    auto root = flatbuffers::GetRoot<schema::Primitive>(buf_bak);
    auto prim = const_cast<schema::Primitive *>(root);

    delete[] buf_bak;
    fbb.Clear();
    return prim;
  }
#endif
  int InferShape(std::vector<lite::tensor::Tensor *> inputs_, std::vector<lite::tensor::Tensor *> outputs_) override;
  std::vector<int> GetBlockShape() const;
  std::vector<int> GetCrops() const;
};
}  // namespace lite
}  // namespace mindspore

#endif  // LITE_MINDSPORE_LITE_C_OPS_BATCH_TO_SPACE_H_
