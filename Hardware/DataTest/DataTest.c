/*
 * Copyright (c) 2025 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include "cmsis_os2.h"
#include "sds_control.h"

// IMU sensor buffer
struct IMU {
  struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
  } accelerometer;
  struct  {
    uint16_t x;
    uint16_t y;
    uint16_t z;
  } gyroscpe;
} imu_buf[30];

// ML output buffer
struct OUT {
  struct {
    uint16_t x;
    uint16_t y;
  } out;
} ml_buf[10];

// Create dummy test data
static void CreateTestData () {
  static uint16_t index_in, index_out;
  uint16_t val;
  int32_t i;

  // Sensor input data
  for (i = 0; i < 30; i++) {
    val = (index_in + i) % 3000;
    imu_buf[i].accelerometer.x = val;
    val = (val + 250) % 3000;
    imu_buf[i].accelerometer.y = 2999 - val;
    val = (val + 300) % 3000;
    imu_buf[i].accelerometer.z = (val < 1500) ? val : (2999 - val);

    val = (val + 150) % 1500;
    imu_buf[i].gyroscpe.x = val;
    val = (val + 70) % 1500;
    imu_buf[i].gyroscpe.y = 1499 - val;
    val = (val + 120) % 1500;
    imu_buf[i].gyroscpe.z = (val < 750) ? val : (1499 - val);
  }
  index_in = (index_in + i) % 3000;

  // ML output data
  for (i = 0; i < 10; i++) {
    val = (index_out + i) % 1000;
    ml_buf[i].out.x = val;
    ml_buf[i].out.y = val % 500;
  }
  index_out = (index_out + i) % 1000;
}

// Thread for generating simulated data
__NO_RETURN void AlgorithmThread (void *argument) {
  uint32_t n, timestamp;
  uint32_t ts, tstamp;
  int32_t diff;
  (void)argument;

  timestamp = osKernelGetTickCount();
  tstamp    = timestamp;

  for (;;) {
    switch (sdsio_state) {
      case SDSIO_OPEN:
        CreateTestData();

        n = sdsRecWrite(recIdDataInput, timestamp, &imu_buf, sizeof(imu_buf));
        SDS_ASSERT(n == sizeof(imu_buf));

        n = sdsRecWrite(recIdDataOutput, timestamp, &ml_buf, sizeof(ml_buf));
        SDS_ASSERT(n == sizeof(ml_buf));
        break;

      case SDSIO_CLOSING:
        // Algorithm execution stopped, transit to halted state
        sdsio_state = SDSIO_HALTED;
        break;
    }

    // Measure jitter in ticks
    ts   = osKernelGetTickCount();
    diff = abs((int32_t)(ts - tstamp));
    if (diff > jitter) jitter = diff;
    tstamp = ts + 10U;

    timestamp += 10U;
    osDelayUntil(timestamp);
  }
}
