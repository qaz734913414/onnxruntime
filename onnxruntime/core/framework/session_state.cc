// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "core/framework/session_state.h"

#include <sstream>

#include "core/common/logging/logging.h"
#include "core/framework/op_kernel.h"

using namespace ::onnxruntime::common;
namespace onnxruntime {

void SessionState::SetGraph(const onnxruntime::Graph& graph) {
  p_graph_ = &graph;
}

const onnxruntime::Graph* SessionState::GetGraph() const {
  return p_graph_;
}

const OpKernel* SessionState::GetKernel(onnxruntime::NodeIndex node_id) const {
  if (session_kernels_.count(node_id) == 0) {
    return nullptr;
  }

  return session_kernels_.find(node_id)->second.get();
}

void SessionState::AddKernel(onnxruntime::NodeIndex node_id, std::unique_ptr<OpKernel> p_kernel) {
  // assumes vector is already resize()'ed to the number of nodes in the graph
  session_kernels_[node_id] = std::move(p_kernel);
}

void SessionState::SetExecutionPlan(std::unique_ptr<SequentialExecutionPlan> p_seq_exec_plan) {
  p_seq_exec_plan_ = std::move(p_seq_exec_plan);
}

const SequentialExecutionPlan* SessionState::GetExecutionPlan() const {
  return p_seq_exec_plan_.get();
}

void SessionState::AddInitializedTensor(int mlvalue_index, const MLValue& mlvalue) {
  LOTUS_ENFORCE(mlvalue_index >= 0 && mlvalue_index <= mlvalue_name_idx_map_.MaxIdx());
  initialized_tensors_.insert({mlvalue_index, mlvalue});
}

const std::unordered_map<int, MLValue>& SessionState::GetInitializedTensors() const {
  return initialized_tensors_;
}

SessionState& SessionState::SetLogger(const Logging::Logger& logger) {
  logger_ = &logger;
  return *this;
}

const Logging::Logger& SessionState::Logger() const {
  // DefaultLogger either throws or returns a valid logger.
  const Logging::Logger* logger = logger_ != nullptr ? logger_ : &Logging::LoggingManager::DefaultLogger();
  return *logger;
}

void SessionState::SetProfiler(Profiling::Profiler& profiler) {
  profiler_ = &profiler;
}

::onnxruntime::Profiling::Profiler& SessionState::Profiler() const {
  return *profiler_;
}

static int64_t CalculateMemoryPatternsKey(const std::vector<TensorShape>& shapes) {
  int64_t key = 0;
  for (auto& shape : shapes) {
    for (auto dim : shape.GetDims())
      key ^= dim;
  }
  return key;
}

const MemoryPatternGroup* SessionState::GetMemoryPatternGroup(const std::vector<TensorShape>& input_shapes) const {
  std::lock_guard<std::mutex> lock(mem_patterns_lock_);
  int64_t key = CalculateMemoryPatternsKey(input_shapes);
  auto it = mem_patterns_.find(key);
  if (it == mem_patterns_.end())
    return nullptr;
  else
    return it->second.get();
}

Status SessionState::UpdateMemoryPatternGroupCache(const std::vector<TensorShape>& input_shape,
                                                   std::unique_ptr<MemoryPatternGroup> mem_patterns) const {
  int64_t key = CalculateMemoryPatternsKey(input_shape);

  std::lock_guard<std::mutex> lock(mem_patterns_lock_);
  auto it = mem_patterns_.find(key);
  if (it == mem_patterns_.end()) {
    mem_patterns_[key] = std::move(mem_patterns);
  }

  return Status::OK();
}

void SessionState::SetEnableMemoryPattern(bool flag) {
  enable_mem_pattern_ = flag;
}

bool SessionState::GetEnableMemoryPattern() const {
  return enable_mem_pattern_;
}

void SessionState::AddInputNameToNodeInfoMapping(const std::string& input_name, const NodeInfo& node_info) {
  input_names_to_nodeinfo_mapping_[input_name].push_back(node_info);
}

common::Status SessionState::GetInputNodeInfo(const std::string& input_name, std::vector<NodeInfo>& node_info_vec) const {
  if (!input_names_to_nodeinfo_mapping_.count(input_name)) {
    return Status(LOTUS, FAIL, "Failed to find input name in the mapping");
  }
  node_info_vec = input_names_to_nodeinfo_mapping_.at(input_name);
  return Status::OK();
}

const SessionState::NameNodeInfoMapType& SessionState::GetInputNodeInfoMap() const {
  return input_names_to_nodeinfo_mapping_;
}

void SessionState::AddOutputNameToNodeInfoMapping(const std::string& output_name, const NodeInfo& node_info) {
  output_names_to_nodeinfo_mapping_[output_name].push_back(node_info);
}

const SessionState::NameNodeInfoMapType& SessionState::GetOutputNodeInfoMap() const {
  return output_names_to_nodeinfo_mapping_;
}

void SessionState::AddSubgraphSessionState(onnxruntime::NodeIndex index,
                                           const std::string& attribute_name,
                                           const SessionState& session_state) {
  auto entry = subgraph_session_states_.find(index);

  // make sure this is new. internal logic error if it is not so using LOTUS_ENFORCE.
  if (entry != subgraph_session_states_.cend()) {
    const auto& existing_entries = entry->second;
    LOTUS_ENFORCE(existing_entries.find(attribute_name) == existing_entries.cend(),
                  "Entry exists in node ", index, " for attribute ", attribute_name);
  }

  subgraph_session_states_[index].insert({attribute_name, &session_state});
}

const SessionState* SessionState::GetSubgraphSessionState(onnxruntime::NodeIndex index,
                                                          const std::string& attribute_name) const {
  const SessionState* session_state = nullptr;

  auto node_entry = subgraph_session_states_.find(index);
  if (node_entry != subgraph_session_states_.cend()) {
    const auto& attribute_state_map{node_entry->second};

    const auto& subgraph_entry = attribute_state_map.find(attribute_name);
    if (subgraph_entry != attribute_state_map.cend()) {
      session_state = subgraph_entry->second;
    }
  }

  return session_state;
}

}  // namespace onnxruntime