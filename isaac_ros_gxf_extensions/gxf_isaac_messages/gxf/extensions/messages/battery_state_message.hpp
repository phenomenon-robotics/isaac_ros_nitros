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

#include "gxf/core/entity.hpp"
#include "gxf/core/expected.hpp"
#include "gxf/std/timestamp.hpp"

namespace nvidia {
namespace isaac {

// Contains battery state information, conforming to
// https://github.com/VDA5050/VDA5050/blob/main/VDA5050_EN.md

struct BatteryState {
  // VDA5050 variables
  double charge;     // percent of charge remaining 0.0 - 100.0
  double voltage;    // current battery voltage in volts
  int8_t health;     // 0-100, battery health, 0 is bad, 100 is good
  bool is_charging;  // Is the battery currently charging?
  uint32_t reach;    // How long in meters the battery drive

  // Extra non VDA5050 info
  int32_t bat_mcurrent;            // battery current (ma)
  int16_t bat_temp;                // battery temp (C)
  bool charge_mode_status;         // Get the charge mode. 0:no charge;1:line charge;2:dock charge;
  bool charge_mos_ctrl_status;     // status of switch for charging MOS, charging:1; no charge:0
  uint16_t charge_connect_status;  // Get the charge connect. 0:no connect;1:charge connect
};

struct BatteryStateMessageParts {
  // Entity for the measurement
  gxf::Entity message;
  // Battery state data
  gxf::Handle<BatteryState> battery_state;
  // Timestamp of publishing and acquisition
  gxf::Handle<gxf::Timestamp> timestamp;
};

gxf::Expected<BatteryStateMessageParts> CreateBatteryStateMessage(gxf_context_t context);
gxf::Expected<BatteryStateMessageParts> GetBatteryStateMessage(gxf::Entity message);

}  // namespace isaac
}  // namespace nvidia
