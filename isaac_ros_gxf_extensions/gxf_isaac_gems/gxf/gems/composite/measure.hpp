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
#pragma once

namespace nvidia {
namespace isaac {
namespace composite {

// Possible measure for an entity in Composite Message
enum class Measure {
  kNone,                 // Unknown
  kTime,                 // Second
  kMass,                 // Kilogram
  kPosition,             // Meter
  kSpeed,                // Meter per second
  kAcceleration,         // Meter per squared second
  kRotation,             // Rotation of 2d or 3d
  kAngularSpeed,         // Radian per second
  kAngularAcceleration,  // Radian per squared second
  kNormal,               // A normal
  kColor,                // A color
  kCurvature,            // Radian per meter
  kCurvatureDerivative,  // Radian per squared meter
};

}  // namespace composite
}  // namespace isaac
}  // namespace nvidia
