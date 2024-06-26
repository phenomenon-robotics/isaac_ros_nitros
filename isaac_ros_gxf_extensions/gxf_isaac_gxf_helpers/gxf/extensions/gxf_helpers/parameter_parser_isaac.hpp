// SPDX-FileCopyrightText: NVIDIA CORPORATION & AFFILIATES
// Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "gems/core/constants.hpp"
#include "gems/core/math/pose2.hpp"
#include "gems/core/math/pose3.hpp"
#include "gems/core/math/types.hpp"
#include "gems/geometry/n_cuboid.hpp"
#include "gems/geometry/n_sphere.hpp"
#include "gems/geometry/polygon.hpp"
#include "gems/uuid/uuid.hpp"
#include "gxf/core/expected.hpp"
#include "gxf/core/gxf.h"
#include "gxf/core/parameter_parser.hpp"
#include "gxf/core/parameter_parser_std.hpp"
#include "yaml-cpp/yaml.h"

namespace nvidia {
namespace gxf {

// Parameter support for std::pair.
// Example format: [object1, object2]
template <typename T, typename N>
struct ParameterParser<std::pair<T, N>> {
  static ::nvidia::gxf::Expected<std::pair<T, N>> Parse(
      gxf_context_t context, gxf_uid_t component_uid,
      const char* key, const YAML::Node& node,
      const std::string& prefix) {
    constexpr int kPairSize = 2;
    if (!node.IsSequence()) {
      GXF_LOG_ERROR("'%s' needs to be a sequence", key);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    const size_t yaml_size = node.size();
    if (static_cast<int>(yaml_size) != kPairSize) {
      GXF_LOG_ERROR("'%s' is a sequence of %zu elements. Expected %d in pair.", key, yaml_size,
          kPairSize);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    std::pair<T, N> result;
    const auto maybe_first = ParameterParser<T>::Parse(context, component_uid, key, node[0],
        prefix);
    if (!maybe_first) {
      return ForwardError(maybe_first);
    }
    result.first = std::move(maybe_first.value());
    const auto maybe_second = ParameterParser<N>::Parse(context, component_uid, key, node[1],
        prefix);
    if (!maybe_second) {
      return ForwardError(maybe_second);
    }
    result.second = std::move(maybe_second.value());
    return result;
  }
};

// Parameter support for Vectors.
// Example format: [1.0, 2.2, -3.7]
template <typename T, int N>
struct ParameterParser<::nvidia::isaac::Vector<T, N>> {
  static ::nvidia::gxf::Expected<::nvidia::isaac::Vector<T, N>> Parse(
      gxf_context_t context, gxf_uid_t component_uid,
      const char* key, const YAML::Node& node,
      const std::string& prefix) {
    if (!node.IsSequence()) {
      GXF_LOG_ERROR("'%s' needs to be a sequence", key);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    const size_t yaml_size = node.size();
    if (N != Eigen::Dynamic && static_cast<int>(yaml_size) != N) {
      GXF_LOG_ERROR("'%s' is a sequence of %zu elements. Expected %d", key, yaml_size, N);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    ::nvidia::isaac::Vector<T, N> result(yaml_size);
    for (size_t i = 0; i < yaml_size; i++) {
      const auto maybe = ParameterParser<T>::Parse(context, component_uid, key, node[i], prefix);
      if (!maybe) {
        return ForwardError(maybe);
      }
      result[i] = std::move(maybe.value());
    }
    return result;
  }
};

// Parameter support for matrices (row-major specification)
// Example format: [[1.0, 2.2, -3.7], [0.3, -1.1, 2.7]]
template <typename T, int N, int M>
struct ParameterParser<::nvidia::isaac::Matrix<T, N, M>> {
  static ::nvidia::gxf::Expected<::nvidia::isaac::Matrix<T, N, M>> Parse(
      gxf_context_t context, gxf_uid_t component_uid,
      const char* key, const YAML::Node& node,
      const std::string& prefix) {
    // Get the number of columns
    if (!node.IsSequence()) {
      GXF_LOG_ERROR("'%s' needs to be a sequence", key);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    const int32_t rows = node.size();
    if (N != Eigen::Dynamic && rows != N) {
      GXF_LOG_ERROR("'%s' is a sequence of %d elements. Expected %d", key, rows, N);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    if (rows <= 0) {
      GXF_LOG_ERROR("Number of rows (%d) must be greater than 0.", rows);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }

    // Get and check the number of columns
    int32_t cols = 0;
    for (int32_t i = 0; i < rows; i++) {
      const auto& sub_node = node[i];
      if (!sub_node.IsSequence()) {
        GXF_LOG_ERROR("'%s' needs to be a sequence of sequences", key);
        return Unexpected{GXF_PARAMETER_PARSER_ERROR};
      }
      const int32_t current_cols = sub_node.size();
      if (M != Eigen::Dynamic && current_cols != M) {
        GXF_LOG_ERROR("'%s' has a sub sequence of %d elements. Expected %d",
                      key, current_cols, M);
        return Unexpected{GXF_PARAMETER_PARSER_ERROR};
      }
      if (current_cols <= 0) {
        GXF_LOG_ERROR("Number of columns (%d) must be greater than 0.", current_cols);
        return Unexpected{GXF_PARAMETER_PARSER_ERROR};
      }
      if (i == 0) {
        cols = current_cols;
      }
      if (current_cols != cols) {
        GXF_LOG_ERROR("All rows must have the same length: %d vs %d", current_cols, cols);
        return Unexpected{GXF_PARAMETER_PARSER_ERROR};
      }
    }

    // Parse elements
    ::nvidia::isaac::Matrix<T, N, M> result(rows, cols);
    for (int32_t i = 0; i < rows; i++) {
      for (int32_t j = 0; j < cols; j++) {
        auto maybe = ParameterParser<T>::Parse(context, component_uid, key, node[i][j], prefix);
        if (!maybe) {
          return ForwardError(maybe);
        }
        result(i, j) = std::move(maybe.value());
      }
    }
    return result;
  }
};

// Parameter support for 2D pose.
// Example formats:
//      translation: [12.2, 8.7]
//      rotation: 3.14 # radians
// or
//      translation: [12.2, 8.7]
//      rotation_deg: 90.0 # degrees
template <typename T>
struct ParameterParser<::nvidia::isaac::Pose2<T>> {
  static ::nvidia::gxf::Expected<::nvidia::isaac::Pose2<T>> Parse(
      gxf_context_t context, gxf_uid_t component_uid, const char* key, const YAML::Node& node,
      const std::string& prefix) {
    if (!node.IsMap()) {
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }

    // translation
    constexpr char kTranslation[] = "translation";
    const auto maybe_node_translation = node[kTranslation];
    if (!maybe_node_translation) {
      GXF_LOG_ERROR("Could not find '%s' in '%s' parameter", kTranslation, key);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    const auto maybe_translation = ParameterParser<::nvidia::isaac::Vector2<T>>::Parse(
        context, component_uid, kTranslation, maybe_node_translation, prefix);
    if (!maybe_translation) {
      return ForwardError(maybe_translation);
    }
    const ::nvidia::isaac::Vector2<T>& translation = maybe_translation.value();

    // angle
    constexpr char kRotation[] = "rotation";
    constexpr char kRotationDeg[] = "rotation_deg";
    T angle = T(0);
    if (const auto maybe_node = node[kRotation]) {
      const auto maybe =
          ParameterParser<T>::Parse(context, component_uid, kRotation, maybe_node, prefix);
      if (!maybe) {
        return ForwardError(maybe);
      }
      angle = maybe.value();
    } else if (const auto maybe_node = node[kRotationDeg]) {
      const auto maybe =
          ParameterParser<T>::Parse(context, component_uid, kRotationDeg, maybe_node, prefix);
      if (!maybe) {
        return ForwardError(maybe);
      }
      angle = ::nvidia::isaac::DegToRad(maybe.value());
    } else {
      GXF_LOG_ERROR("Could not find '%s' or '%s' in '%s' parameter", kRotation, kRotationDeg, key);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }

    return ::nvidia::isaac::Pose2<T>::FromXYA(translation.x(), translation.y(), angle);
  }
};

// Parameter support for 3D pose.
// Example formats:
//      translation: [2.2, 8.7, 0.0]
//      rotation_rpy: [0.0, 90.0, -180.0] # degrees
// or
//      translation: [2.2, 8.7, 0.0]
//      rotation: [-0.393, -0.469, -0.725, 0.314] # (w, x, y, z) values forming the quaternion
template <typename T>
struct ParameterParser<::nvidia::isaac::Pose3<T>> {
  static ::nvidia::gxf::Expected<::nvidia::isaac::Pose3<T>> Parse(
      gxf_context_t context, gxf_uid_t component_uid, const char* key, const YAML::Node& node,
      const std::string& prefix) {
    if (!node.IsMap()) {
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }

    // translation
    constexpr char kTranslation[] = "translation";
    const auto maybe_node_translation = node[kTranslation];
    if (!maybe_node_translation) {
      GXF_LOG_ERROR("Could not find '%s' in '%s' parameter", kTranslation, key);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    const auto maybe_translation = ParameterParser<::nvidia::isaac::Vector3<T>>::Parse(
        context, component_uid, kTranslation, maybe_node_translation, prefix);
    if (!maybe_translation) {
      return ForwardError(maybe_translation);
    }
    const ::nvidia::isaac::Vector3<T>& translation = maybe_translation.value();

    // rotation
    constexpr char kRotation[] = "rotation";
    constexpr char kRotationRpy[] = "rotation_rpy";
    ::nvidia::isaac::SO3<T> rotation = ::nvidia::isaac::SO3<T>::Identity();
    if (const auto maybe_node = node[kRotation]) {
      const auto maybe = ParameterParser<::nvidia::isaac::Vector4<T>>::Parse(context, component_uid,
                                                                     kRotation, maybe_node,
                                                                     prefix);
      if (!maybe) {
        return ForwardError(maybe);
      }
      rotation = ::nvidia::isaac::SO3<T>::FromQuaternion(
          {maybe.value()[0], maybe.value()[1], maybe.value()[2], maybe.value()[3]});
    } else if (const auto maybe_node = node[kRotationRpy]) {
      const auto maybe = ParameterParser<::nvidia::isaac::Vector3<T>>::Parse(context, component_uid,
                                                                     kRotationRpy, maybe_node,
                                                                     prefix);
      if (!maybe) {
        return ForwardError(maybe);
      }
      rotation = ::nvidia::isaac::SO3<T>::FromEulerAnglesRPY(
          ::nvidia::isaac::DegToRad(maybe.value()[0]),
          ::nvidia::isaac::DegToRad(maybe.value()[1]),
          ::nvidia::isaac::DegToRad(maybe.value()[2]));
    } else {
      GXF_LOG_ERROR("Could not find '%s' or '%s' in '%s' parameter", kRotation, kRotationRpy, key);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }

    return ::nvidia::isaac::Pose3<T>{rotation, translation};
  }
};

// Parameter support for NSphere.
// Example formats:
//      center: [X, Y [, ...]]
//      radius: 3.0
template <typename T, int N>
struct ParameterParser<::nvidia::isaac::geometry::NSphere<T, N>> {
  static ::nvidia::gxf::Expected<::nvidia::isaac::geometry::NSphere<T, N>> Parse(
      gxf_context_t context, gxf_uid_t component_uid,
      const char* key, const YAML::Node& node, const std::string& prefix) {
    if (!node.IsMap()) {
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }

    // center
    constexpr char kCenter[] = "center";
    const auto maybe_node_center = node[kCenter];
    if (!maybe_node_center) {
      GXF_LOG_ERROR("Could not find '%s' in '%s' parameter", kCenter, key);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    const auto maybe_center = ParameterParser<::nvidia::isaac::Vector<T, N>>::Parse(
        context, component_uid, kCenter, maybe_node_center, prefix);
    if (!maybe_center) {
      return ForwardError(maybe_center);
    }
    const ::nvidia::isaac::Vector<T, N>& center = maybe_center.value();

    // radius
    constexpr char kRadius[] = "radius";
    const auto maybe_node_radius = node[kRadius];
    if (!maybe_node_radius) {
      GXF_LOG_ERROR("Could not find '%s' in '%s' parameter", kRadius, key);
      return Unexpected{GXF_PARAMETER_PARSER_ERROR};
    }
    const auto maybe_radius =
        ParameterParser<T>::Parse(context, component_uid, kRadius, maybe_node_radius, prefix);
    if (!maybe_radius) {
      return ForwardError(maybe_radius);
    }
    const T radius = maybe_radius.value();

    return ::nvidia::isaac::geometry::NSphere<T, N>{center, radius};
  }
};

// Parameter support for NCuboid (where N is the dimension of the cuboid). The input is a std::array
// of length N, for which each component contains an the minimum and maximum bound for Nth dimension
// Example: [[-1.0, 1.0], [-2.0, 2.0]] represents a rectangle with the x dimension spanning -1.0 to
// 1.0 and the y dimension spanning -2.0 to 2.0.
template <typename T, int N>
struct ParameterParser<::nvidia::isaac::geometry::NCuboid<T, N>> {
  static ::nvidia::gxf::Expected<::nvidia::isaac::geometry::NCuboid<T, N>> Parse(
    gxf_context_t context, gxf_uid_t component_uid, const char* key, const YAML::Node& node,
    const std::string& prefix) {
    const auto maybe = ParameterParser<std::array<::nvidia::isaac::Vector2<T>, N>>::Parse(
        context, component_uid, key, node, prefix);
    if (!maybe) {
      return ForwardError(maybe);
    }
    return ::nvidia::isaac::geometry::NCuboid<T, N>::FromBoundingCuboid(maybe.value());
  }
};

// Parameter support for Polygon2. Example format:
// [[60, -104], [32, -96], [-6, -88], [-52, -81], [-95, -82]]
// This would form a polygon with 5 points in 2D. Each point consist of [x, y] values.
template <typename T>
struct ParameterParser<::nvidia::isaac::geometry::Polygon2<T>> {
  static ::nvidia::gxf::Expected<::nvidia::isaac::geometry::Polygon2<T>> Parse(
    gxf_context_t context, gxf_uid_t component_uid, const char* key, const YAML::Node& node,
    const std::string& prefix) {
    const auto maybe = ParameterParser<std::vector<::nvidia::isaac::Vector2<T>>>::Parse(
        context, component_uid, key, node, prefix);
    if (!maybe) {
      return ForwardError(maybe);
    }
    return ::nvidia::isaac::geometry::Polygon2<T>{maybe.value()};
  }
};

// Parameter support for LineSegment. Example format:
//   [ [-100.0, 0.0], [20.0, 5.0] ]
// This would form a line segment between (-100.0, 0.0) and (20.0, 5.0).
// Each point consist of (x, y) values.
template <typename T, int N>
struct ParameterParser<::nvidia::isaac::geometry::LineSegment<T, N>> {
  static ::nvidia::gxf::Expected<::nvidia::isaac::geometry::LineSegment<T, N>> Parse(
    gxf_context_t context, gxf_uid_t component_uid, const char* key, const YAML::Node& node,
    const std::string& prefix) {
    const auto maybe = ParameterParser<std::array<::nvidia::isaac::Vector<T, N>, 2>>::Parse(
        context, component_uid, key, node, prefix);
    if (!maybe) {
      return ForwardError(maybe);
    }
    return ::nvidia::isaac::geometry::LineSegment<T, N>::FromPoints(
        maybe.value()[0], maybe.value()[1]);
  }
};

// Parameter support for Uuid.
template <>
struct ParameterParser<::nvidia::isaac::Uuid> {
  static ::nvidia::gxf::Expected<::nvidia::isaac::Uuid> Parse(
      gxf_context_t context, gxf_uid_t component_uid, const char* key,
      const YAML::Node& node, const std::string& prefix) {
    auto str = ParameterParser<std::string>::Parse(context, component_uid, key, node, prefix);
    if (!str) {
      return ForwardError(str);
    }
    return ::nvidia::isaac::Uuid::FromString(str.value());
  }
};

}  // namespace gxf
}  // namespace nvidia
