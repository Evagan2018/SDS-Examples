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

#include "play_rec_management.h"

#include <stdio.h>
#include <stdbool.h>

#include "RTE_Components.h"

#include "cmsis_os2.h"
#include "cmsis_vio.h"

#include "sds_play.h"
#include "sds_rec.h"

// Configuration

// SDS Player/Recorder data buffers size
#ifndef PLAY_BUF_SIZE_MODEL_IN
#define PLAY_BUF_SIZE_MODEL_IN          8192U
#endif
#ifndef REC_BUF_SIZE_MODEL_OUT
#define REC_BUF_SIZE_MODEL_OUT          576U
#endif

// SDS Player/Recorder IO thresholds
#ifndef PLAY_IO_THRESHOLD_MODEL_IN
#define PLAY_IO_THRESHOLD_MODEL_IN      (PLAY_BUF_SIZE_MODEL_IN/2)
#endif
#ifndef REC_IO_THRESHOLD_MODEL_OUT
#define REC_IO_THRESHOLD_MODEL_OUT      (REC_BUF_SIZE_MODEL_OUT-64U)
#endif

#ifdef  RTE_SDS_IO_SOCKET
extern  int32_t  socket_startup (void);
#endif

// SDS error information
#ifndef NDEBUG
sdsError_t       sdsError = { 0U, 0U, NULL, 0U };
#endif

// Player/Recorder active status
volatile uint8_t playRecActive = 0U;

// User request for Player/Recorder to stop
volatile uint8_t playRecStop   = 0U;

// Player/Recorder identifiers
sdsPlayId_t      playIdModelInput = NULL;
sdsRecId_t       recIdModelOutput = NULL;

// SDS Player/Recorder buffers
static uint8_t   sds_play_buf_model_in[PLAY_BUF_SIZE_MODEL_IN];
static uint8_t   sds_rec_buf_model_out[REC_BUF_SIZE_MODEL_OUT];

// Player event callback
static void player_event_callback (sdsPlayId_t id, uint32_t event) {
  if ((event & SDS_PLAY_EVENT_IO_ERROR) != 0U) {
    SDS_ASSERT(false);
  }
}

// Recorder event callback
static void recorder_event_callback (sdsRecId_t id, uint32_t event) {
  if ((event & SDS_REC_EVENT_IO_ERROR) != 0U) {
    SDS_ASSERT(false);
  }
}

// Playback/Recording control thread function.
// Toggle playback/recording via USER push-button.
// Toggle LED0 every 1 second to see that the thread is alive.
// Turn on LED1 when playback/recording is started, turn it off when playback/recording is stopped.
__NO_RETURN void threadPlayRecManagement (void *argument) {
  uint8_t btn_val;
  uint8_t btn_prev = 0U;
  uint8_t led0_cnt = 0U;
  uint8_t led0_val = 0U;
  int32_t status;

#ifdef RTE_SDS_IO_SOCKET
  // Initialize socket interface
  status = socket_startup();
  SDS_ASSERT(status == 0);
#endif

  // Initialize SDS player
  status = sdsPlayInit(player_event_callback);
  SDS_ASSERT(status == SDS_PLAY_OK);

  // Initialize SDS recorder
  status = sdsRecInit(recorder_event_callback);
  SDS_ASSERT(status == SDS_REC_OK);

  for (;;) {
    // Toggle LED0 every 1 second
    if (led0_cnt++ >= 10U) {
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
        if (playRecActive == 0U) {
          // Start playback of previously recorded Model Input data
          playIdModelInput = sdsPlayOpen("ModelInput",
                                          sds_play_buf_model_in,
                                          sizeof(sds_play_buf_model_in),
                                          PLAY_IO_THRESHOLD_MODEL_IN);
          SDS_ASSERT(playIdModelInput != NULL);

          // Start recording of Model Output data
          recIdModelOutput = sdsRecOpen("ModelOutput",
                                         sds_rec_buf_model_out,
                                         sizeof(sds_rec_buf_model_out),
                                         REC_IO_THRESHOLD_MODEL_OUT);
          SDS_ASSERT(recIdModelOutput != NULL);

          if ((playIdModelInput != NULL) && (recIdModelOutput != NULL)) {
            (void)osDelay(200U);        // Allow player to load initial data for playback

            playRecActive = 1U;

            // If playback/recording was started turn LED1 on
            vioSetSignal(vioLED1, vioLEDon);
          }
        }
      }
    }
    if (playRecStop != 0U) {            // If user request to stop Playback/Recording
      playRecStop = 0U;

      // Stop playback of previously recorded Model Input data
      status = sdsPlayClose(playIdModelInput);
      SDS_ASSERT(status == SDS_PLAY_OK);

      // Stop recording of Model Output data
      status = sdsRecClose(recIdModelOutput);
      SDS_ASSERT(status == SDS_REC_OK);

      // Turn LED1 off
      vioSetSignal(vioLED1, vioLEDoff);
    }

    (void)osDelay(100U);                // Delay for button debouncing
  }
}
