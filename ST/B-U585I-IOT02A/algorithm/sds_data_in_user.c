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

#include <stddef.h>
#include <stdio.h>
#include "sds_algorithm_config.h"
#include "sds_data_in.h"
#include "cmsis_os2.h"
#include "cmsis_compiler.h"
#include "vstream_accelerometer.h"


// Configuration

#ifndef SENSOR_RAW_ITEMS_PER_SAMPLE
#define SENSOR_RAW_ITEMS_PER_SAMPLE     (3)     // For raw accelerometer data there are x, y, z (3) items
#endif

#ifndef SENSOR_RAW_BYTES_PER_SAMPLE
#define SENSOR_RAW_BYTES_PER_SAMPLE     (6)     // For raw accelerometer data there are 3 items of 2 bytes per sample
#endif

#ifndef SENSOR_COND_BYTES_PER_SAMPLE
#define SENSOR_COND_BYTES_PER_SAMPLE    (12)    // For conditioned sensor data there are 3 items of 1 float per sample
#endif

#ifndef SENSOR_SAMPLES_PER_SLICE
#define SENSOR_SAMPLES_PER_SLICE        (125)   // Number of samples per inference slice (raw or conditioned)
#endif

// Constants

// Number of items per inference slice
#define SENSOR_SLICE_SIZE_IN_ITEMS      (SENSOR_SAMPLES_PER_SLICE * SENSOR_RAW_ITEMS_PER_SAMPLE)

// Number of bytes per inference slice of raw sensor data
#define SENSOR_RAW_SLICE_SIZE_IN_BYTES  (SENSOR_SAMPLES_PER_SLICE * SENSOR_RAW_BYTES_PER_SAMPLE)

// Number of bytes per inference slice for sensor data acquisition
#define SENSOR_COND_SLICE_SIZE_IN_BYTES (SENSOR_SAMPLES_PER_SLICE * SENSOR_COND_BYTES_PER_SAMPLE)

// Thread flag for signaling sensor data available
#define SENSOR_DATA_READY_FLAG          1U


// Raw sensor data sample structure
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} accelerometer_sample_t;

// Raw sensor data (2 * slice)
static uint8_t vstream_buf[SENSOR_RAW_SLICE_SIZE_IN_BYTES*2] __ALIGNED(4);

// ML input data (1 slice of items in float format)
static float scaled_sensor_data[SENSOR_SLICE_SIZE_IN_ITEMS];

// Event flags
static osEventFlagsId_t evt_id_EventFlags = NULL;

// Pointer to vStream driver
static vStreamDriver_t *ptrDriver_vStreamAccelerometer = &Driver_vStreamAccelerometer;

// Function that sends event when data is available with vStream
static void vStreamSensorEvent (uint32_t event_flags) {

  if ((event_flags & VSTREAM_EVENT_DATA) != 0U) {
    // New block of data is ready
    osEventFlagsSet(evt_id_EventFlags, SENSOR_DATA_READY_FLAG);
  }
  if ((event_flags & VSTREAM_EVENT_OVERFLOW) != 0U) {
    printf("Warning: Accelerometer overflow event!\r\n");
  }
}

/**
  \fn           int32_t InitInputData (void)
  \brief        Initialize system for acquiring input data.
  \return       0 on success; -1 on error
*/
int32_t InitInputData (void) {

  evt_id_EventFlags = osEventFlagsNew(NULL);
  if (evt_id_EventFlags == NULL) {
    return -1;
  }
  if (ptrDriver_vStreamAccelerometer->Initialize(vStreamSensorEvent) != 0) {
    return -1;
  }
  if (ptrDriver_vStreamAccelerometer->SetBuf(&vstream_buf, sizeof(vstream_buf), SENSOR_RAW_SLICE_SIZE_IN_BYTES) != 0) {
    return -1;
  }
  if (ptrDriver_vStreamAccelerometer->Start(VSTREAM_MODE_CONTINUOUS) != 0) {
    return -1;
  }

  return 0;
}

/**
  \fn           int32_t GetInputData (uint8_t *buf, uint32_t max_len)
  \brief        Get input data block as required for algorithm under test.
  \details      Size of this block has to match size expected by algorithm under test.
  \param[out]   buf             pointer to memory buffer for acquiring input data
  \param[in]    max_len         maximum number of bytes of input data to acquire
  \return       number of data bytes returned; -1 on error
*/
int32_t GetInputData (uint8_t *buf, uint32_t max_len) {
  accelerometer_sample_t *ptr_acc_sample;
  float                  *ptr_scaled_sensor_data;

  // Check input parameters
  if ((buf == NULL) || (max_len == 0U)) {
    return -1;
  }

  // Check if buffer can fit expected data
  if (max_len < SENSOR_COND_SLICE_SIZE_IN_BYTES) {
    return -1;
  }

  uint32_t flags = osEventFlagsWait(evt_id_EventFlags, SENSOR_DATA_READY_FLAG, osFlagsWaitAny, osWaitForever);

  if (((flags & osFlagsError)           == 0U) &&         // If not an error and
      ((flags & SENSOR_DATA_READY_FLAG) != 0U)) {         // if flag is sensor data ready

    // Get pointer to sensor data
    ptr_acc_sample = (accelerometer_sample_t *)ptrDriver_vStreamAccelerometer->GetBlock();

    // Set pointer for scaled sensor data
    ptr_scaled_sensor_data = (float *)buf;

    for (uint32_t i = 0U; i < SENSOR_SAMPLES_PER_SLICE; i++) {
      // Convert each sample value for x, y, z from int16 to float scaled to range used during model training
      ptr_scaled_sensor_data[0]  = ((float)ptr_acc_sample->x) / 1638.4f;
      ptr_scaled_sensor_data[1]  = ((float)ptr_acc_sample->y) / 1638.4f;
      ptr_scaled_sensor_data[2]  = ((float)ptr_acc_sample->z) / 1638.4f;
      ptr_acc_sample            += 1U;
      ptr_scaled_sensor_data    += SENSOR_RAW_ITEMS_PER_SAMPLE;

      // Used for debugging, to visually check that data is plausible
      // printf("Acc x=%i, y=%i, z=%i\r\n",         ptr_acc_sample->x           ,         ptr_acc_sample->y           ,         ptr_acc_sample->z           );
      // printf("Acc x=%f, y=%f, z=%f\r\n", ((float)ptr_acc_sample->x) / 1638.4f, ((float)ptr_acc_sample->y) / 1638.4f, ((float)ptr_acc_sample->z) / 1638.4f);
    }

    // Release data block
    ptrDriver_vStreamAccelerometer->ReleaseBlock();
  }

  return SENSOR_COND_SLICE_SIZE_IN_BYTES;
}
