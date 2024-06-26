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

#ifndef CUSTOM_NITROS_IMAGE__GPU_IMAGE_BUILDER_NODE_HPP_
#define CUSTOM_NITROS_IMAGE__GPU_IMAGE_BUILDER_NODE_HPP_

#include <memory>

#include "rclcpp/rclcpp.hpp"

#include "isaac_ros_managed_nitros/managed_nitros_publisher.hpp"

#include "sensor_msgs/msg/image.hpp"
#include "isaac_ros_nitros_image_type/nitros_image.hpp"

namespace custom_nitros_image
{

class GpuImageBuilderNode : public rclcpp::Node
{
public:
  explicit GpuImageBuilderNode(const rclcpp::NodeOptions options = rclcpp::NodeOptions());

  ~GpuImageBuilderNode();

private:
  void InputCallback(const sensor_msgs::msg::Image::SharedPtr msg);

  // Subscription to input Image messages
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_;

  // Publisher for output NitrosImage messages
  std::shared_ptr<nvidia::isaac_ros::nitros::ManagedNitrosPublisher<
      nvidia::isaac_ros::nitros::NitrosImage>> nitros_pub_;
};

}  // namespace custom_nitros_image

#endif  // CUSTOM_NITROS_IMAGE__GPU_IMAGE_BUILDER_NODE_HPP_
