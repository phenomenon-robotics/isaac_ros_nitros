// SPDX-FileCopyrightText: NVIDIA CORPORATION & AFFILIATES
// Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ISAAC_ROS_NITROS__NITROS_CONTEXT_HPP_
#define ISAAC_ROS_NITROS__NITROS_CONTEXT_HPP_

#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "gxf/core/gxf.h"

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/header.hpp"


namespace nvidia
{
namespace isaac_ros
{
namespace nitros
{

// An abstraction layer for GXF context
class NitrosContext
{
public:
  // Constructor
  NitrosContext();

  // Setter for graph_namespace_
  void setGraphNamespace(const std::string & graph_namespace);

  // Setter for node_ that is used to get ROS logger if set
  void setNode(const rclcpp::Node * node);

  // Getter for the created local context
  gxf_context_t getContext();

  // Get the pointer of the specified component
  gxf_result_t getComponentPointer(
    const std::string & entity_name,
    const std::string & component_name,
    const std::string & component_type,
    void ** pointer);

  // Get the EID for the given entity name
  gxf_result_t getEid(
    const std::string & entity_name,
    gxf_uid_t & eid);

  // Get the CID for a component of the given type in the given entity name
  gxf_result_t getCid(
    const std::string & entity_name,
    const std::string & component_type,
    gxf_uid_t & cid);

  // Get the CID for a component of the given name and type in the given entity name
  gxf_result_t getCid(
    const std::string & entity_name,
    const std::string & component_name,
    const std::string & component_type,
    gxf_uid_t & cid);

  // Override a parameter value in the graph to be loaded
  void preLoadGraphSetParameter(
    const std::string & entity_name,
    const std::string & component_name,
    const std::string & parameter_name,
    const std::string & value
  );

  // Load an extension so file
  gxf_result_t loadExtension(
    const std::string & base_dir,
    const std::string & extension);

  // Load a list of extension so files
  gxf_result_t loadExtensions(
    const std::string & base_dir,
    const std::vector<std::string> & extensions);

  // Load the given graph(s) to the context
  gxf_result_t loadApplication(const std::string & list_of_files);

  // Activate and asynchronously run the loaded graph
  gxf_result_t runGraphAsync();

  // Terminate the running graph(s)
  gxf_result_t destroy();

  // Get a timestamp stored in the given entity and assign to the given ROS header
  gxf_result_t getEntityTimestamp(
    const gxf_uid_t eid,
    std_msgs::msg::Header & ros_header);

  // Set GXF log level for the context
  void setExtensionLogSeverity(gxf_severity_t severity_level);

  // Setter for the default CUDA memory pool size on device 0.
  // Only take effect when the requested CUDA memory pool size is larger than the current
  gxf_result_t setCUDAMemoryPoolSize(uint64_t cuda_mem_pool_size);

  // APIs for setting a parameter value in a given component
  gxf_result_t setParameterInt64(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const int64_t parameter_value);
  gxf_result_t setParameterInt64(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const int64_t parameter_value);

  gxf_result_t setParameterUInt64(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const uint64_t parameter_value);
  gxf_result_t setParameterUInt64(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const uint64_t parameter_value);

  gxf_result_t setParameterInt32(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const int32_t parameter_value);
  gxf_result_t setParameterInt32(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const int32_t parameter_value);

  gxf_result_t setParameterUInt32(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const uint32_t parameter_value);
  gxf_result_t setParameterUInt32(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const uint32_t parameter_value);

  gxf_result_t setParameterUInt16(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const uint16_t parameter_value);
  gxf_result_t setParameterUInt16(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const uint16_t parameter_value);

  gxf_result_t setParameterFloat32(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const float parameter_value);
  gxf_result_t setParameterFloat32(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const float parameter_value);

  gxf_result_t setParameterFloat64(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const double parameter_value);
  gxf_result_t setParameterFloat64(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const double parameter_value);

  gxf_result_t setParameterStr(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const std::string & parameter_value);
  gxf_result_t setParameterStr(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const std::string & parameter_value);

  gxf_result_t setParameterHandle(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const gxf_uid_t & uid);
  gxf_result_t setParameterHandle(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const gxf_uid_t & uid);

  gxf_result_t setParameterBool(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const bool parameter_value);
  gxf_result_t setParameterBool(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const bool parameter_value);

  gxf_result_t setParameter1DStrVector(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const std::vector<std::string> & parameter_value);
  gxf_result_t setParameter1DStrVector(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    const std::vector<std::string> & parameter_value);

  gxf_result_t setParameter1DInt32Vector(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    std::vector<int32_t> & parameter_value);
  gxf_result_t setParameter1DInt32Vector(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    std::vector<int32_t> & parameter_value);

  gxf_result_t setParameter1DInt64Vector(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    std::vector<int64_t> & parameter_value);
  gxf_result_t setParameter1DInt64Vector(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    std::vector<int64_t> & parameter_value);

  gxf_result_t setParameter1DFloat64Vector(
    const std::string & entity_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    std::vector<double> & parameter_value);
  gxf_result_t setParameter1DFloat64Vector(
    const std::string & entity_name,
    const std::string & codelet_name,
    const std::string & codelet_type,
    const std::string & parameter_name,
    std::vector<double> & parameter_value);

private:
  // Get the entity name with the configured graph namespace
  std::string getNamespacedEntityName(const std::string & entity_name);

  // Get the node's logger if set. Forward to rclcpp::get_logger() otherwise.
  rclcpp::Logger get_logger();

  // The associated ROS node (for logging purpose)
  const rclcpp::Node * node_ = nullptr;

  // Shared context across all NitrosContext
  static gxf_context_t main_context_;
  static gxf_context_t shared_context_;
  static std::mutex shared_context_mutex_;

  // A shared loaded extension list for avoiding duplicate loadings
  static std::set<std::string> loaded_extension_file_paths_;

  // Local context created from the shared context
  gxf_context_t context_;

  // Graph namespace for avoiding conflict names in the shared context
  std::string graph_namespace_;

  // GXF graph parameter overriding strings
  std::vector<std::string> graph_param_override_string_list_;

  // Extension log severity level
  static gxf_severity_t extension_log_severity_;
};

}  // namespace nitros
}  // namespace isaac_ros
}  // namespace nvidia

#endif  // ISAAC_ROS_NITROS__NITROS_CONTEXT_HPP_
