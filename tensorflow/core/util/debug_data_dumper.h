/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_CORE_UTIL_DEBUG_DATA_DUMPER_H_
#define TENSORFLOW_CORE_UTIL_DEBUG_DATA_DUMPER_H_

#include <string>

#include "absl/container/flat_hash_map.h"
#include "tensorflow/core/platform/mutex.h"

namespace tensorflow {

class Graph;

////////////////////////////////////////////////////////////////////////////////
// This class is responsible for dumping debugging data (e.g., GraphDef, MLIR).
//
// To use DebugDataDumper, take the following steps:
// * Set envvar TF_DUMP_GRAPH_PREFIX to your target dump directory.
// * Set envvar TF_DUMP_GRAPH_NAME_FILTER to '*' to dump all graphs,
//   or a name filter to dump graphs with a name containing it.
//
// The dumped graphs then can be found in your target dump directory.
// The filename of the dump looks like this:
// <name>.<order-id>.<tag>
//
// This is what each field means:
// * <name>     : The name of your dump.
// * <order-id> : The order of dumps of a specific name.
//                Lower orders are executed before higher orders.
// * <tag>      : The tag of your dump, e.g., your pass name.
//
// Example dump files are:
// __inference_train_step_441.0.before_pre_placement_passes.pbtxt
// __inference_train_step_441.1.before_placer.pbtxt
// __inference_train_step_441.2.before_post_placement_passes.pbtxt
// __inference_train_step_441.3.before_graph_optimization.pbtxt
// __inference_train_step_441.4.after_graph_optimization.pbtxt
// __inference_train_step_441.5.before_post_rewrite_for_exec_passes.pbtxt
////////////////////////////////////////////////////////////////////////////////
class DebugDataDumper {
 public:
  // Get the singleton instance.
  static DebugDataDumper* Global();

  // Dump a graph.
  void DumpGraph(const std::string& name, const Graph* graph,
                 const std::string& tag);

 private:
  // Get next dump id for a name.
  int GetNextDumpId(const std::string& name) { return dump_order_ids_[name]++; }

  // A dict to maintain the mapping from dump name to its current dump id.
  absl::flat_hash_map<std::string, int> dump_order_ids_;

  // A mutex to make sure this is thread safe.
  tensorflow::mutex lock_;
};

}  // namespace tensorflow

#endif  // TENSORFLOW_CORE_UTIL_DEBUG_DATA_DUMPER_H_
