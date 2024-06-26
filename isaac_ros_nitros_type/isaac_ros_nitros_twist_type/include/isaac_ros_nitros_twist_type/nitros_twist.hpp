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

#ifndef ISAAC_ROS_NITROS_TWIST_TYPE__NITROS_TWIST_HPP_
#define ISAAC_ROS_NITROS_TWIST_TYPE__NITROS_TWIST_HPP_
/*
 * Type adaptation for:
 *   Nitros type: NitrosTwist
 *   ROS type:    geometry_msgs::msg::Twist
 */

#include <string>

#include "isaac_ros_nitros/types/nitros_format_agent.hpp"

#include "rclcpp/type_adapter.hpp"
#include "geometry_msgs/msg/twist.hpp"


namespace nvidia
{
namespace isaac_ros
{
namespace nitros
{

// Type forward declaration
struct NitrosTwist;

// Formats
struct nitros_twist_t
{
  using MsgT = NitrosTwist;
  static const inline std::string supported_type_name = "nitros_twist";
};

// NITROS data type registration factory
NITROS_TYPE_FACTORY_BEGIN(NitrosTwist)
// Supported data formats
NITROS_FORMAT_FACTORY_BEGIN()
NITROS_FORMAT_ADD(nitros_twist_t)
NITROS_FORMAT_FACTORY_END()
// Required extensions
NITROS_TYPE_EXTENSION_FACTORY_BEGIN()
NITROS_TYPE_EXTENSION_ADD("isaac_ros_gxf", "gxf/lib/serialization/libgxf_serialization.so")
// for nvidia::isaac::StringProvider
NITROS_TYPE_EXTENSION_ADD("gxf_isaac_gxf_helpers", "gxf/lib/libgxf_isaac_gxf_helpers.so")
// for nvidia::isaac::SightReceiver
NITROS_TYPE_EXTENSION_ADD("gxf_isaac_sight", "gxf/lib/libgxf_isaac_sight.so")
// for nvidia::isaac::PoseTree
NITROS_TYPE_EXTENSION_ADD("gxf_isaac_atlas", "gxf/lib/libgxf_isaac_atlas.so")
NITROS_TYPE_EXTENSION_ADD("gxf_isaac_messages", "gxf/lib/libgxf_isaac_messages.so")
NITROS_TYPE_EXTENSION_FACTORY_END()
NITROS_TYPE_FACTORY_END()

}  // namespace nitros
}  // namespace isaac_ros
}  // namespace nvidia


template<>
struct rclcpp::TypeAdapter<
  nvidia::isaac_ros::nitros::NitrosTwist,
  geometry_msgs::msg::Twist>
{
  using is_specialized = std::true_type;
  using custom_type = nvidia::isaac_ros::nitros::NitrosTwist;
  using ros_message_type = geometry_msgs::msg::Twist;

  static void convert_to_ros_message(
    const custom_type & source,
    ros_message_type & destination);

  static void convert_to_custom(
    const ros_message_type & source,
    custom_type & destination);
};

RCLCPP_USING_CUSTOM_TYPE_AS_ROS_MESSAGE_TYPE(
  nvidia::isaac_ros::nitros::NitrosTwist,
  geometry_msgs::msg::Twist);

#endif  // ISAAC_ROS_NITROS_TWIST_TYPE__NITROS_TWIST_HPP_
