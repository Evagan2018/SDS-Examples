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

#include "sds_algorithm_config.h"
#include "sds_algorithm.h"

#include "ei_run_classifier.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "model-parameters/model_variables.h"


static const uint8_t *ptr_in_data = NULL;
static signal_t features_signal;


// Get sensor data for inference
int sensor_get_data (size_t offset, size_t length, float *out_ptr) {

  if (ptr_in_data == NULL) {
    return -1;
  }

  memcpy(out_ptr, ptr_in_data + (offset * sizeof(float)), length * sizeof(float));
  ptr_in_data = NULL;

  return 0;
}

/**
  \fn           int32_t InitAlgorithm (void)
  \brief        Initialize algorithm under test.
  \return       0 on success; -1 on error
*/
extern "C" int32_t InitAlgorithm (void) {

  features_signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
  features_signal.get_data = &sensor_get_data;

  return 0;
}

/**
  \fn           int32_t ExecuteAlgorithm (const uint8_t *in_buf, uint32_t in_num, uint8_t *out_buf, uint32_t out_num)
  \brief        Execute algorithm under test.
  \param[in]    in_buf          pointer to memory buffer containing input data for algorithm
  \param[in]    in_num          number of data bytes in input data buffer (in bytes)
  \param[out]   out_buf         pointer to memory buffer for returning algorithm output
  \param[in]    out_num         maximum number of data bytes returned as algorithm output (in bytes)
  \return       0 on success; -1 on error
*/
extern "C" int32_t ExecuteAlgorithm (const uint8_t *in_buf, uint32_t in_num, uint8_t *out_buf, uint32_t out_num) {
  ei_impulse_result_t result = {nullptr};
  float *ptr_out_data;

  if ((in_buf == NULL) || (in_num == 0U)) {
    return -1;
  }

  // Register input data for sensor_get_data callback
  ptr_in_data = in_buf;

  // Run the classifier
  EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);

  if (run_classifier(&features_signal, &result, false) != 0) {
    ei_printf("ERR: Failed to run classifier\n");
    return -1;
  }

  // Display classification results to the STDIO
  display_results(&ei_default_impulse, &result);

  // Output, for example:
  // idle: 0.996094
  // snake: 0.000000
  // updown: 0.000000
  // wave: 0.000000

  if ((out_buf != NULL) && (out_num >= (EI_CLASSIFIER_NN_OUTPUT_COUNT * sizeof(float)))) {
    ptr_out_data = (float *)out_buf;
    for (uint8_t i = 0U; i < EI_CLASSIFIER_NN_OUTPUT_COUNT; i++) {
      ptr_out_data[i] = result.classification[i].value;
    }
  }

  return 0;
}
