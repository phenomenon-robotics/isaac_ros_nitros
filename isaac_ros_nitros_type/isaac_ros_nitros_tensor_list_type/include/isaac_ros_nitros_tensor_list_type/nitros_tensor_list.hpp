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

#ifndef ISAAC_ROS_NITROS_TENSOR_LIST_TYPE__NITROS_TENSOR_LIST_HPP_
#define ISAAC_ROS_NITROS_TENSOR_LIST_TYPE__NITROS_TENSOR_LIST_HPP_
/*
 * Type adaptation for:
 *   Nitros type: NitrosTensorList
 *   ROS type:    isaac_ros_tensor_list_interfaces::msg::TensorList
 */

#include <cuda_runtime.h>

#include <string>

#include "isaac_ros_nitros/types/nitros_format_agent.hpp"
#include "isaac_ros_nitros/types/nitros_type_base.hpp"
#include "isaac_ros_tensor_list_interfaces/msg/tensor_list.hpp"

#include "rclcpp/type_adapter.hpp"


namespace nvidia
{
namespace isaac_ros
{
namespace nitros
{

// Type forward declaration
struct NitrosTensorList;

// Formats
struct nitros_tensor_list_nchw_t
{
  using MsgT = NitrosTensorList;
  static const inline std::string supported_type_name = "nitros_tensor_list_nchw";
};

struct nitros_tensor_list_nhwc_t
{
  using MsgT = NitrosTensorList;
  static const inline std::string supported_type_name = "nitros_tensor_list_nhwc";
};

struct nitros_tensor_list_nchw_rgb_f32_t
{
  using MsgT = NitrosTensorList;
  static const inline std::string supported_type_name = "nitros_tensor_list_nchw_rgb_f32";
};

struct nitros_tensor_list_nhwc_rgb_f32_t
{
  using MsgT = NitrosTensorList;
  static const inline std::string supported_type_name = "nitros_tensor_list_nhwc_rgb_f32";
};

struct nitros_tensor_list_nchw_bgr_f32_t
{
  using MsgT = NitrosTensorList;
  static const inline std::string supported_type_name = "nitros_tensor_list_nchw_bgr_f32";
};

struct nitros_tensor_list_nhwc_bgr_f32_t
{
  using MsgT = NitrosTensorList;
  static const inline std::string supported_type_name = "nitros_tensor_list_nhwc_bgr_f32";
};

// NITROS data type registration factory
NITROS_TYPE_FACTORY_BEGIN(NitrosTensorList)
// Supported data formats
NITROS_FORMAT_FACTORY_BEGIN()
NITROS_FORMAT_ADD(nitros_tensor_list_nchw_t)
NITROS_FORMAT_ADD(nitros_tensor_list_nhwc_t)
NITROS_FORMAT_ADD(nitros_tensor_list_nchw_rgb_f32_t)
NITROS_FORMAT_ADD(nitros_tensor_list_nhwc_rgb_f32_t)
NITROS_FORMAT_ADD(nitros_tensor_list_nchw_bgr_f32_t)
NITROS_FORMAT_ADD(nitros_tensor_list_nhwc_bgr_f32_t)
NITROS_FORMAT_FACTORY_END()
// Default compatible data format
NITROS_DEFAULT_COMPATIBLE_FORMAT(nitros_tensor_list_nchw_t)
// Required extensions
NITROS_TYPE_EXTENSION_FACTORY_BEGIN()
NITROS_TYPE_EXTENSION_FACTORY_END()
NITROS_TYPE_FACTORY_END()

}  // namespace nitros
}  // namespace isaac_ros
}  // namespace nvidia


template<>
struct rclcpp::TypeAdapter<
  nvidia::isaac_ros::nitros::NitrosTensorList,
  isaac_ros_tensor_list_interfaces::msg::TensorList>
{
  using is_specialized = std::true_type;
  using custom_type = nvidia::isaac_ros::nitros::NitrosTensorList;
  using ros_message_type = isaac_ros_tensor_list_interfaces::msg::TensorList;

  static void convert_to_ros_message(
    const custom_type & source,
    ros_message_type & destination);

  static void convert_to_custom(
    const ros_message_type & source,
    custom_type & destination);
};

RCLCPP_USING_CUSTOM_TYPE_AS_ROS_MESSAGE_TYPE(
  nvidia::isaac_ros::nitros::NitrosTensorList,
  isaac_ros_tensor_list_interfaces::msg::TensorList);

#endif  // ISAAC_ROS_NITROS_TENSOR_LIST_TYPE__NITROS_TENSOR_LIST_HPP_
