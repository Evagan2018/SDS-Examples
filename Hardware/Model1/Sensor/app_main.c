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

#include "app_main.h"

#include "ei_main.h"
#include "rec_management.h"
#include "sensor_vstream.h"

// Thread identifiers
osThreadId_t thrId_threadSensor        = NULL;
osThreadId_t thrId_threadRecManagement = NULL;
osThreadId_t thrId_threadMLInference   = NULL;

/*-----------------------------------------------------------------------------
 * ML inference thread
 *----------------------------------------------------------------------------*/
__NO_RETURN void threadMLInference (void *argument) {
  (void)argument;

  ei_main();

  for (;;);
}

/*-----------------------------------------------------------------------------
 * Application main function
 *----------------------------------------------------------------------------*/
int app_main (void) {
  osKernelInitialize();
  thrId_threadSensor        = osThreadNew(threadSensor,        NULL, NULL);
  thrId_threadRecManagement = osThreadNew(threadRecManagement, NULL, NULL);
  thrId_threadMLInference   = osThreadNew(threadMLInference,   NULL, NULL);
  osKernelStart();
  return 0;
}
