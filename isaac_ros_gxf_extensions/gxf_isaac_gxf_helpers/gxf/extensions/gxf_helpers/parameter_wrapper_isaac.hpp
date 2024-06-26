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
#include "gxf/core/parameter_wrapper.hpp"
#include "yaml-cpp/yaml.h"

namespace nvidia {
namespace gxf {

// uint8_t by default is interpreted as unsigned char, however value above 127 are not valid
// and json failed to serialize as the string is not a valid utf-8 string.
template<>
struct ParameterWrapper<uint8_t> {
  static Expected<YAML::Node> Wrap(gxf_context_t context, uint8_t value) {
    YAML::Node node(static_cast<int>(value));
    return node;
  }
};

template <typename T, typename N>
struct ParameterWrapper<std::pair<T, N>> {
  static Expected<YAML::Node> Wrap(
      gxf_context_t context, const std::pair<T, N>& value) {
    YAML::Node node(YAML::NodeType::Sequence);

    const auto maybe_first = ParameterWrapper<T>::Wrap(context, value.first);
    if (!maybe_first) {
      return ForwardError(maybe_first);
    }
    node.push_back(maybe_first.value());

    const auto maybe_second = ParameterWrapper<N>::Wrap(context, value.second);
    if (!maybe_second) {
      return ForwardError(maybe_second);
    }
    node.push_back(maybe_second.value());

    return node;
  }
};

template <typename T, int N>
struct ParameterWrapper<::nvidia::isaac::Vector<T, N>> {
  static Expected<YAML::Node> Wrap(
      gxf_context_t context, const ::nvidia::isaac::Vector<T, N>& value) {
    YAML::Node node(YAML::NodeType::Sequence);

    for (uint32_t i = 0; i < value.size(); i++) {
      const auto maybe = ParameterWrapper<T>::Wrap(context, value[i]);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
  }
};

// Parameter support for matrices (row-major specification)
// Example format: [[1.0, 2.2, -3.7], [0.3, -1.1, 2.7]]
template <typename T, int N, int M>
struct ParameterWrapper<::nvidia::isaac::Matrix<T, N, M>> {
  static Expected<YAML::Node> Wrap(
      gxf_context_t context, const ::nvidia::isaac::Matrix<T, N, M>& value) {
    YAML::Node node(YAML::NodeType::Sequence);

    for (int32_t i = 0; i < N; i++) {
      for (int32_t j = 0; j < M; j++) {
        auto maybe = ParameterWrapper<T>::Wrap(context, value(i, j));
        if (!maybe) {
          return ForwardError(maybe);
        }
        node.push_back(maybe.value());
      }
    }
    return node;
  }
};

// Parameter support for matrices (row-major specification)
// Example format: [[1.0, 2.2, -3.7], [0.3, -1.1, 2.7]]
template <typename T, int N, int M>
struct ParameterWrapper<std::vector<::nvidia::isaac::Matrix<T, N, M>>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context,
                                   const std::vector<::nvidia::isaac::Matrix<T, N, M>>& value) {
    YAML::Node node(YAML::NodeType::Sequence);

    for (auto &v : value) {
      const auto maybe = ParameterWrapper<::nvidia::isaac::Matrix<T, N, M>>::Wrap(context, v);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
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
struct ParameterWrapper<::nvidia::isaac::Pose2<T>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context, const ::nvidia::isaac::Pose2<T>& value) {
    YAML::Node node(YAML::NodeType::Map);

    // translation
    const auto maybe_translation =
      ParameterWrapper<Eigen::Matrix<T, 2, 1>>::Wrap(context, value.translation);
    if (!maybe_translation) {
      return ForwardError(maybe_translation);
    }
    node["translation"] = maybe_translation.value();

    // rotation
    const auto maybe_rotation =
        ParameterWrapper<T>::Wrap(context, value.rotation.angle());
    if (!maybe_rotation) {
      return ForwardError(maybe_rotation);
    }
    node["rotation"] = maybe_rotation.value();
    return node;
  }
};

// Support for std::vector of Pose2
template <typename T>
struct ParameterWrapper<std::vector<::nvidia::isaac::Pose2<T>>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context,
                                   const std::vector<::nvidia::isaac::Pose2<T>>& value) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (auto &v : value) {
      const auto maybe = ParameterWrapper<::nvidia::isaac::Pose2<T>>::Wrap(context, v);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
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
struct ParameterWrapper<::nvidia::isaac::Pose3<T>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context, const ::nvidia::isaac::Pose3<T>& value) {
    YAML::Node node(YAML::NodeType::Map);
    // translation
    const auto maybe_translation =
      ParameterWrapper<Eigen::Matrix<T, 3, 1>>::Wrap(context, value.translation);
    if (!maybe_translation) {
      return ForwardError(maybe_translation);
    }
    node["translation"] = maybe_translation.value();

    // rotation
    Eigen::Matrix<T, 3, 1> angle = value.rotation.eulerAnglesRPY();
    angle(0, 0) = ::nvidia::isaac::RadToDeg(angle(0, 0));
    angle(1, 0) = ::nvidia::isaac::RadToDeg(angle(1, 0));
    angle(2, 0) = ::nvidia::isaac::RadToDeg(angle(2, 0));
    const auto maybe_rotation =
        ParameterWrapper<Eigen::Matrix<T, 3, 1>>::Wrap(context, angle);
    if (!maybe_rotation) {
      return ForwardError(maybe_rotation);
    }
    node["rotation"] = maybe_rotation.value();
    return node;
  }
};

// Support for std::array of Pose3
template <typename T>
struct ParameterWrapper<std::array<::nvidia::isaac::Pose3<T>, 4>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context,
                                   const std::array<::nvidia::isaac::Pose3<T>, 4>& value) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (uint32_t i = 0; i < 4; i++) {
      const auto maybe = ParameterWrapper<::nvidia::isaac::Pose3<T>>::Wrap(context, value[i]);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
  }
};

template <typename T>
struct ParameterWrapper<std::vector<::nvidia::isaac::Pose3<T>>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context,
                                   const std::vector<::nvidia::isaac::Pose3<T>>& value) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (auto &v : value) {
      const auto maybe = ParameterWrapper<::nvidia::isaac::Pose3<T>>::Wrap(context, v);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
  }
};

// Parameter support for NSphere.
// Example formats:
//      center: [X, Y [, ...]]
//      radius: 3.0
template <typename T, int N>
struct ParameterWrapper<::nvidia::isaac::geometry::NSphere<T, N>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context,
                                   const ::nvidia::isaac::geometry::NSphere<T, N>& value) {
    YAML::Node node(YAML::NodeType::Map);

    // center
    const auto maybe_center =
      ParameterWrapper<::nvidia::isaac::Vector<T, N>>::Wrap(context, value.center);
    if (!maybe_center) {
      return ForwardError(maybe_center);
    }
    node["center"] = maybe_center.value();

    // radius
    const auto maybe_radius =
      ParameterWrapper<T>::Wrap(context, value.radius);
    if (!maybe_radius) {
      return ForwardError(maybe_radius);
    }
    node["radius"] = maybe_radius.value();

    return node;
  }
};

// Support for std::array of NSphere
template <typename T, int N>
struct ParameterWrapper<std::vector<::nvidia::isaac::geometry::NSphere<T, N>>> {
  static Expected<YAML::Node> Wrap(
      gxf_context_t context,
      const std::vector<::nvidia::isaac::geometry::NSphere<T, N>>& value) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (auto &v : value) {
      const auto maybe =
        ParameterWrapper<::nvidia::isaac::geometry::NSphere<T, N>>::Wrap(context, v);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
  }
};

// Parameter support for NCuboid (where N is the dimension of the cuboid). The input is a std::array
// of length N, for which each component contains an the minimum and maximum bound for Nth dimension
// Example: [[-1.0, 1.0], [-2.0, 2.0]] represents a rectangle with the x dimension spanning -1.0 to
// 1.0 and the y dimension spanning -2.0 to 2.0.
template <typename T, int N>
struct ParameterWrapper<::nvidia::isaac::geometry::NCuboid<T, N>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context,
                                   const ::nvidia::isaac::geometry::NCuboid<T, N>& value) {
    YAML::Node node(YAML::NodeType::Sequence);

    // min
    const auto maybe_min =
      ParameterWrapper<::nvidia::isaac::Vector<T, N>>::Wrap(context, value.min());
    if (!maybe_min) {
      return ForwardError(maybe_min);
    }
    node.push_back(maybe_min.value());

    // max
    const auto maybe_max =
      ParameterWrapper<::nvidia::isaac::Vector<T, N>>::Wrap(context, value.max());
    if (!maybe_max) {
      return ForwardError(maybe_max);
    }
    node.push_back(maybe_max.value());
    return node;
  }
};

// Support for std::vector of NCuboid
template <typename T, int N>
struct ParameterWrapper<std::vector<::nvidia::isaac::geometry::NCuboid<T, N>>> {
  static Expected<YAML::Node> Wrap(
      gxf_context_t context,
      const std::vector<::nvidia::isaac::geometry::NCuboid<T, N>>& value) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (auto &v : value) {
      const auto maybe =
        ParameterWrapper<::nvidia::isaac::geometry::NCuboid<T, N>>::Wrap(context, v);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
  }
};

// Parameter support for Polygon2. Example format:
// [[60, -104], [32, -96], [-6, -88], [-52, -81], [-95, -82]]
// This would form a polygon with 5 points in 2D. Each point consist of [x, y] values.
template <typename T>
struct ParameterWrapper<::nvidia::isaac::geometry::Polygon2<T>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context,
                                   const ::nvidia::isaac::geometry::Polygon2<T>& value) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (auto &point : value.points) {
      const auto maybe = ParameterWrapper<::nvidia::isaac::Vector<T, 2>>::Wrap(context, point);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
  }
};

// Support for std::vector of Polygon2
template <typename T>
struct ParameterWrapper<std::vector<::nvidia::isaac::geometry::Polygon2<T>>> {
  static Expected<YAML::Node> Wrap(
      gxf_context_t context,
      const std::vector<::nvidia::isaac::geometry::Polygon2<T>>& value) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (auto &v : value) {
      const auto maybe = ParameterWrapper<::nvidia::isaac::geometry::Polygon2<T>>::Wrap(context, v);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
  }
};

// Parameter support for LineSegment. Example format:
//   [ [-100.0, 0.0], [20.0, 5.0] ]
// This would form a line segment between (-100.0, 0.0) and (20.0, 5.0).
// Each point consist of (x, y) values.
template <typename T, int N>
struct ParameterWrapper<::nvidia::isaac::geometry::LineSegment<T, N>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context,
                                   const ::nvidia::isaac::geometry::LineSegment<T, N>& value) {
    YAML::Node node(YAML::NodeType::Sequence);

    // vector_a
    const auto maybe_a = ParameterWrapper<::nvidia::isaac::Vector<T, N>>::Wrap(context, value.a());
    if (!maybe_a) {
      return ForwardError(maybe_a);
    }
    node.push_back(maybe_a.value());

    // vector_b
    const auto maybe_b = ParameterWrapper<::nvidia::isaac::Vector<T, N>>::Wrap(context, value.b());
    if (!maybe_b) {
      return ForwardError(maybe_b);
    }
    node.push_back(maybe_b.value());
    return node;
  }
};

// Support for std::vector of LineSegment
template <typename T, int N>
struct ParameterWrapper<std::vector<::nvidia::isaac::geometry::LineSegment<T, N>>> {
  static Expected<YAML::Node> Wrap(
      gxf_context_t context,
      const std::vector<::nvidia::isaac::geometry::LineSegment<T, N>>& value) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (auto &v : value) {
      const auto maybe =
        ParameterWrapper<::nvidia::isaac::geometry::LineSegment<T, N>>::Wrap(context, v);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
  }
};

// Parameter support for std::vector.
// Example format: [1.0, 2.2, -3.7]
template<typename T>
struct ParameterWrapper<std::vector<T>> {
  static Expected<YAML::Node> Wrap(gxf_context_t context, const std::vector<T>& value) {
    YAML::Node node(YAML::NodeType::Sequence);
    for (auto &h : value) {
      auto maybe = ParameterWrapper<T>::Wrap(context, h);
      if (!maybe) {
        return ForwardError(maybe);
      }
      node.push_back(maybe.value());
    }
    return node;
  }
};

// Parameter support for Uuid.
template<>
struct ParameterWrapper<::nvidia::isaac::Uuid> {
  static Expected<YAML::Node> Wrap(gxf_context_t context, const ::nvidia::isaac::Uuid& uuid) {
    return ParameterWrapper<std::string>::Wrap(context, uuid.str());
  }
};

}  // namespace gxf
}  // namespace nvidia
