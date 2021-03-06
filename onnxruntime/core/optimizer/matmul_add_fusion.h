// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "core/optimizer/graph_transformer.h"

namespace onnxruntime {

class MatMulAddFusion : public GraphTransformer {
 public:
  MatMulAddFusion(const std::unordered_set<std::string>& compatible_execution_providers = {}) noexcept 
      : GraphTransformer("MatMulAddFusion", "Fusing MatMul and Add into Gemm", compatible_execution_providers) {}

  Status ApplyImpl(Graph& graph, bool& modified, int graph_level) const override;
};

}  // namespace onnxruntime
