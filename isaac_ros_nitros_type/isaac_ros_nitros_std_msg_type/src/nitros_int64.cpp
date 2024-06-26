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

#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include "gxf/core/entity.hpp"
#include "gxf/core/gxf.h"
#pragma GCC diagnostic pop

#include "isaac_ros_nitros_std_msg_type/nitros_int64.hpp"
#include "isaac_ros_nitros/types/type_adapter_nitros_context.hpp"

#include "rclcpp/rclcpp.hpp"


void rclcpp::TypeAdapter<
  nvidia::isaac_ros::nitros::NitrosInt64,
  std_msgs::msg::Int64>::convert_to_ros_message(
  const custom_type & source,
  ros_message_type & destination)
{
  nvidia::isaac_ros::nitros::nvtxRangePushWrapper(
    "NitrosInt64::convert_to_ros_message",
    nvidia::isaac_ros::nitros::CLR_PURPLE);

  RCLCPP_DEBUG(
    rclcpp::get_logger("NitrosInt64"),
    "[convert_to_ros_message] Conversion started for handle=%ld", source.handle);

  auto context = nvidia::isaac_ros::nitros::GetTypeAdapterNitrosContext().getContext();
  auto gxf_message = nvidia::gxf::Entity::Shared(context, source.handle);
  if (!gxf_message) {
    std::stringstream error_msg;
    error_msg <<
      "[convert_to_ros_message] Error getting message entity: " <<
      GxfResultStr(gxf_message.error());
    RCLCPP_ERROR(
      rclcpp::get_logger("NitrosInt64"), error_msg.str().c_str());
    throw std::runtime_error(error_msg.str().c_str());
  }

  auto data = gxf_message.value().get<int64_t>("payload");
  if (!data) {
    std::stringstream error_msg;
    error_msg <<
      "[convert_to_ros_message] Error getting data from message entity: " <<
      GxfResultStr(data.error());
    RCLCPP_ERROR(
      rclcpp::get_logger("NitrosInt64"), error_msg.str().c_str());
    throw std::runtime_error(error_msg.str().c_str());
  }
  destination.data = *data->get();

  nvidia::isaac_ros::nitros::nvtxRangePopWrapper();
}

void rclcpp::TypeAdapter<
  nvidia::isaac_ros::nitros::NitrosInt64,
  std_msgs::msg::Int64>::convert_to_custom(
  const ros_message_type & source,
  custom_type & destination)
{
  nvidia::isaac_ros::nitros::nvtxRangePushWrapper(
    "NitrosInt64::convert_to_custom",
    nvidia::isaac_ros::nitros::CLR_PURPLE);

  RCLCPP_DEBUG(
    rclcpp::get_logger("NitrosInt64"),
    "[convert_to_custom] Conversion started");

  // Create an entity for storing the actual data in the context
  auto context = nvidia::isaac_ros::nitros::GetTypeAdapterNitrosContext().getContext();
  auto gxf_message = nvidia::gxf::Entity::New(context);
  if (!gxf_message) {
    std::stringstream error_msg;
    error_msg <<
      "[convert_to_custom] Error initializing new message entity: " <<
      GxfResultStr(gxf_message.error());
    RCLCPP_ERROR(
      rclcpp::get_logger("NitrosInt64"), error_msg.str().c_str());
    throw std::runtime_error(error_msg.str().c_str());
  }
  auto payload = gxf_message.value().add<int64_t>("payload");
  if (!payload) {
    std::stringstream error_msg;
    error_msg <<
      "[convert_to_custom] Error creating component for int64_t payload: " <<
      GxfResultStr(payload.error());
    RCLCPP_ERROR(
      rclcpp::get_logger("NitrosInt64"), error_msg.str().c_str());
    throw std::runtime_error(error_msg.str().c_str());
  }
  *payload->get() = source.data;

  // Set the entity's pointer (EID) in a Nitros type data struct
  destination.handle = gxf_message->eid();
  destination.data_format_name = "nitros_int64";

  // Increase the reference count for the created entity so it doesn't get destroyed after
  // exiting this function
  GxfEntityRefCountInc(context, gxf_message->eid());

  nvidia::isaac_ros::nitros::nvtxRangePopWrapper();
}
