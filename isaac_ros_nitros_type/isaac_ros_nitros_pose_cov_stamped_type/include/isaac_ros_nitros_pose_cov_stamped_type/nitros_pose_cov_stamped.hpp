// SPDX-FileCopyrightText: NVIDIA CORPORATION & AFFILIATES
// Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef ISAAC_ROS_NITROS_POSE_COV_STAMPED_TYPE__NITROS_POSE_COV_STAMPED_HPP_
#define ISAAC_ROS_NITROS_POSE_COV_STAMPED_TYPE__NITROS_POSE_COV_STAMPED_HPP_
/*
 * Type adaptation for:
 *   Nitros type: NitrosPoseCovStamped
 *   ROS type:    geometry_msgs::msg::PoseWithCovarianceStamped
 */

#include <string>

#include "isaac_ros_nitros/types/nitros_format_agent.hpp"

#include "rclcpp/type_adapter.hpp"
#include "geometry_msgs/msg/pose_with_covariance_stamped.hpp"


namespace nvidia
{
namespace isaac_ros
{
namespace nitros
{

// Type forward declaration
struct NitrosPoseCovStamped;

// Formats
struct nitros_pose_cov_stamped_t
{
  using MsgT = NitrosPoseCovStamped;
  static const inline std::string supported_type_name = "nitros_pose_cov_stamped";
};

// NITROS data type registration factory
NITROS_TYPE_FACTORY_BEGIN(NitrosPoseCovStamped)
// Supported data formats
NITROS_FORMAT_FACTORY_BEGIN()
NITROS_FORMAT_ADD(nitros_pose_cov_stamped_t)
NITROS_FORMAT_FACTORY_END()
// Required extensions
NITROS_TYPE_EXTENSION_FACTORY_BEGIN()
NITROS_TYPE_EXTENSION_ADD("isaac_ros_gxf", "gxf/lib/serialization/libgxf_serialization.so")
NITROS_TYPE_EXTENSION_ADD("gxf_isaac_messages", "gxf/lib/libgxf_isaac_messages.so")
NITROS_TYPE_EXTENSION_FACTORY_END()
NITROS_TYPE_FACTORY_END()

}  // namespace nitros
}  // namespace isaac_ros
}  // namespace nvidia


template<>
struct rclcpp::TypeAdapter<
  nvidia::isaac_ros::nitros::NitrosPoseCovStamped,
  geometry_msgs::msg::PoseWithCovarianceStamped>
{
  using is_specialized = std::true_type;
  using custom_type = nvidia::isaac_ros::nitros::NitrosPoseCovStamped;
  using ros_message_type = geometry_msgs::msg::PoseWithCovarianceStamped;

  static void convert_to_ros_message(
    const custom_type & source,
    ros_message_type & destination);

  static void convert_to_custom(
    const ros_message_type & source,
    custom_type & destination);
};

RCLCPP_USING_CUSTOM_TYPE_AS_ROS_MESSAGE_TYPE(
  nvidia::isaac_ros::nitros::NitrosPoseCovStamped,
  geometry_msgs::msg::PoseWithCovarianceStamped);

#endif  // ISAAC_ROS_NITROS_POSE_COV_STAMPED_TYPE__NITROS_POSE_COV_STAMPED_HPP_
