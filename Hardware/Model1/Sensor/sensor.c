/*
 * Copyright (c) 2022-2025 Arm Limited. All rights reserved.
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

#include "sensor.h"

#include <stdio.h>
#include <string.h>

#include "app_main.h"
#include "ei_main.h"

#include "cmsis_os2.h"

#include "rec_management.h"

#include "sensor_drv.h"
#include "sensor_config.h"

// Configuration
#ifndef SENSOR_POLLING_INTERVAL
#define SENSOR_POLLING_INTERVAL         19U     // 19.23ms (sensor is sampled at 52 Hz)
#endif

#ifndef SENSOR_ITEMS_PER_SAMPLE
#define SENSOR_ITEMS_PER_SAMPLE         3U      // For accelerometer there are x, y, z (3) items per sample
#endif

#ifndef SENSOR_BUF_SIZE
#define SENSOR_BUF_SIZE                 128U    // Temporary sensor data buffer
#endif

#define SENSOR_SLICE_SIZE               (125 * SENSOR_ITEMS_PER_SAMPLE) // ML input data size (slice for inference)

// Timestamp of read of input data for inference
uint32_t rec_timestamp;

// Sensor identifiers
static sensorId_t sensorId_accelerometer;

// Sensor configuration
static sensorConfig_t *sensorConfig_accelerometer;

// Temporary sensor buffer
static uint8_t sensorBuf[SENSOR_BUF_SIZE];

// Raw sensor data sample structure
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} accelerometer_sample_t;

// ML input data
static float sensor_data[SENSOR_SLICE_SIZE];

// ML input data busy flag
static volatile uint8_t sensor_data_busy = 0U;

// ML input data filling offset
static uint32_t sensor_data_offset = 0U;


// Get sensor data for inference
int sensor_get_data (size_t offset, size_t length, float *out_ptr)
{
    memcpy(out_ptr, sensor_data + offset, length * sizeof(float));

    // If recording is active then record model input data
    if (recActive != 0U) {
      rec_timestamp = osKernelGetTickCount();
      uint32_t num  = sdsRecWrite(recIdModelInput, rec_timestamp, sensor_data + offset, length * sizeof(float));
      SDS_ASSERT(num == (length * sizeof(float)));
    }

    sensor_data_busy = 0U;

    return 0;
}

// Thread function that activates sensor and does polling of sensor data
__NO_RETURN void threadSensor (void *argument) {
  uint32_t num;
  uint32_t num_max;
  uint32_t timestamp;
  accelerometer_sample_t *ptr_acc_sample;
  float   *ptr_sensor_data;
  (void)   argument;

  // Get sensor identifier
  sensorId_accelerometer = sensorGetId("Accelerometer");

  // Get sensor configuration
  sensorConfig_accelerometer = sensorGetConfig(sensorId_accelerometer);

  // Activate accelerometer sensor
  sensorEnable(sensorId_accelerometer);

  timestamp = osKernelGetTickCount();
  for (;;) {
    if ((sensor_data_busy == 0U) &&                                     // If sensor data is not busy do we can write to it and
        (sensorGetStatus(sensorId_accelerometer).active != 0U)) {       // if sensor is active

      // Calculate maximum number of samples we want to read
      num_max = sizeof(sensorBuf) / sensorConfig_accelerometer->sample_size;
      if (num_max > ((SENSOR_SLICE_SIZE - sensor_data_offset)/3U)) {
        num_max = ((SENSOR_SLICE_SIZE - sensor_data_offset)/3U);
      }

      // Read samples
      num = sensorReadSamples(sensorId_accelerometer, num_max, sensorBuf, sizeof(sensorBuf));

      if (num != 0U) {                  // If more than 0 samples were read from sensor
        ptr_acc_sample  = (accelerometer_sample_t *)sensorBuf;
        ptr_sensor_data = &sensor_data[sensor_data_offset];
        for (uint32_t i = 0U; i < num; i ++) {
          // Convert each sample value for x, y, z from int16 to float scaled to range used during model training
          ptr_sensor_data[0]  = ((float)ptr_acc_sample->x) / 1638.4f;
          ptr_sensor_data[1]  = ((float)ptr_acc_sample->y) / 1638.4f;
          ptr_sensor_data[2]  = ((float)ptr_acc_sample->z) / 1638.4f;
          ptr_acc_sample     += 1U;
          ptr_sensor_data    += SENSOR_ITEMS_PER_SAMPLE;
          sensor_data_offset += SENSOR_ITEMS_PER_SAMPLE;

          // Used for debugging to check data
          // printf("Acc x=%i, y=%i, z=%i\r\n",         ptr_acc_sample->x           ,         ptr_acc_sample->y           ,         ptr_acc_sample->z           );
          // printf("Acc x=%f, y=%f, z=%f\r\n", ((float)ptr_acc_sample->x) / 1638.4f, ((float)ptr_acc_sample->y) / 1638.4f, ((float)ptr_acc_sample->z) / 1638.4f);
        }

        if (sensor_data_offset == SENSOR_SLICE_SIZE) {  // If all the data for inference was prepared
          sensor_data_offset = 0U;
          sensor_data_busy   = 1U;              // Prevent further write to sensor data until data was consumed by ML and sent to SDS Recorder if necessary

          // Inform main ML thread that sensor data is ready
          osThreadFlagsSet(thrId_threadMLInference, SENSOR_DATA_READY_FLAG);
        }
      }
    }

    timestamp += SENSOR_POLLING_INTERVAL;
    osDelayUntil(timestamp);            // Wait until next sampling interval
  }
}
