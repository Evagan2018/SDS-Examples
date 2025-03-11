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

#include "rec_management.h"

#include <stdio.h>
#include <stdbool.h>

#include "RTE_Components.h"

#include "cmsis_os2.h"
#include "cmsis_vio.h"

#include "sds_rec.h"
#include "DataTest.h"

// Configuration

// SDS Recorder data buffers
#ifndef REC_BUF_SIZE_DATA_IN
#define REC_BUF_SIZE_DATA_IN            8192U
#endif
#ifndef REC_BUF_SIZE_DATA_OUT
#define REC_BUF_SIZE_DATA_OUT           1536U
#endif

// SDS Recorder IO thresholds
#ifndef REC_IO_THRESHOLD_DATA_IN
#define REC_IO_THRESHOLD_DATA_IN        7400U
#endif
#ifndef REC_IO_THRESHOLD_DATA_OUT
#define REC_IO_THRESHOLD_DATA_OUT       1400U
#endif

extern  int32_t  socket_startup (void);


// Recorder error information
sdsError_t       sdsError = { 0U, 0U, NULL, 0U };

// Recorder active status
volatile uint8_t recActive = 0U;

// Idle time counter
volatile uint32_t idle_cnt;

// Recorder identifiers
sdsRecId_t       recIdDataInput  = NULL;
sdsRecId_t       recIdDataOutput = NULL;

// Recorder buffers
static uint8_t   rec_buf_data_in [REC_BUF_SIZE_DATA_IN];
static uint8_t   rec_buf_data_out[REC_BUF_SIZE_DATA_OUT];

// Recorder event callback
static void recorder_event_callback (sdsRecId_t id, uint32_t event) {
  if ((event & SDS_REC_EVENT_IO_ERROR) != 0U) {
    SDS_ASSERT(false);
  }
}

// Recording control thread function.
// Toggle recording via USER push-button.
// Toggle LED0 every 1 second to see that the thread is alive.
// Turn on LED1 when recording is started, turn it off when recording is stopped.
__NO_RETURN void threadRecManagement (void *argument) {
  uint8_t btn_val;
  uint8_t btn_prev = 0U;
  uint8_t led0_cnt = 0U;
  uint8_t led0_val = 0U;
  int32_t status;
  uint32_t no_load_cnt, prev_cnt;
  uint32_t timestamp, cnt = 0U;

  // Initialize idle counter
  idle_cnt = 0U;
  osDelay(1000U);
  no_load_cnt = idle_cnt;

#ifdef RTE_SDS_IO_SOCKET
  // Initialize socket interface
  status = socket_startup();
  SDS_ASSERT(status == 0);
#endif

  // Initialize SDS recorder
  status = sdsRecInit(recorder_event_callback);
  SDS_ASSERT(status == SDS_REC_OK);

  osThreadNew(threadTestData, NULL, NULL);

  timestamp = osKernelGetTickCount();
  prev_cnt  = idle_cnt;

  for (;;) {
    // Toggle LED0 every 1 second
    if (++led0_cnt >= 10U) {
      led0_cnt  = 0U;
      led0_val ^= 1U;
      vioSetSignal(vioLED0, led0_val);
    }

    // Monitor user button
    btn_val = vioGetSignal(vioBUTTON0);
    if (btn_val != btn_prev) {
      // If push-button state has changed
      btn_prev = btn_val;
      if (btn_val == vioBUTTON0) {      // If push-button is pressed
        if (recActive == 0U) {
          // Start recording of Input data
          recIdDataInput  = sdsRecOpen("DataInput",
                                        rec_buf_data_in,
                                        sizeof(rec_buf_data_in),
                                        REC_IO_THRESHOLD_DATA_IN);
          SDS_ASSERT(recIdDataInput != NULL);

          // Start recording of Output data
          recIdDataOutput = sdsRecOpen("DataOutput",
                                        rec_buf_data_out,
                                        sizeof(rec_buf_data_out),
                                        REC_IO_THRESHOLD_DATA_OUT);
          SDS_ASSERT(recIdDataOutput != NULL);

          // If recording was started turn LED1 on
          vioSetSignal(vioLED1, vioLEDon);
          printf("Start Recording\n");
          recActive = 1U;
        } else {
          recActive = 0U;

          // Stop recording of Input data
          status = sdsRecClose(recIdDataInput);
          SDS_ASSERT(status == SDS_REC_OK);

          // Stop recording of Output data
          status = sdsRecClose(recIdDataOutput);
          SDS_ASSERT(status == SDS_REC_OK);

          // If recording was stopped turn LED1 off
          vioSetSignal(vioLED1, vioLEDoff);
          printf("Stop Recording\n");
        }
      }
    }
    timestamp += 100U;
    osDelayUntil(timestamp);            // Delay for button debouncing

    if (++cnt == 10U) {
      printf("%d%% idle\n",(idle_cnt - prev_cnt) / (no_load_cnt / 100U));
      prev_cnt = idle_cnt;
      cnt      = 0U;
    }
  }
}

// Measure system idle time
__NO_RETURN void osRtxIdleThread(void *argument) {
  (void)argument;

  for (;;) {
    idle_cnt++;
  }
}
