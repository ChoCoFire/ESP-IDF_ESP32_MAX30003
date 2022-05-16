/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "main_functions.h"

#include <string.h>

#include "constants.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "model.h"
#include "nvs_flash.h"
#include "output_handler.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

extern "C" {
// #include "foo.h" //a C header, so wrap it in extern "C"
#include "max30003/max30003.h"
#include "signal/aaa_c_connector.h"
#include "signal/normalize.h"
#include "wave/wavelib.h"
#include "wifi/rest.h"
}

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;

int signal_inx = 0;
static const int signal_per_loop = 750;
static const int signal_target = 750;

double signal_arr[signal_target];

std::string str = "";
std::string create_by = "esp01";
std::string edit_by = "esp01";

std::string result = "";
std::string result_acc = "";

bool loopEcg = true;

// Create an area of memory to use for input, output, and intermediate arrays.
// Minimum arena size, at the time of writing. After allocating tensors
// you can retrieve this value by invoking interpreter.arena_used_bytes().
const int kModelArenaSize = 2468;
// Extra headroom for model + alignment + future interpreter changes.
const int kExtraArenaSize = 560 + 16 + 100;
const int kTensorArenaSize = kModelArenaSize + kExtraArenaSize;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

double* preProcess(double signal[]) {
  /********************************
  setup parameter
  *********************************/
  int lb = -1;  // lower bound
  int ub = 2;   // upper bound
  int signal_size = signal_target;

  double* result;

  wave_object obj;
  wt_object wt;
  char* name = "db6";
  double* inp;
  int wlvl = 2;  // wavelet lv

  /********************************
  Start Detrend
  *********************************/
  getDetrend(signal, signal_size);

  /********************************
  Start Normalize
  *********************************/
  result = getNormalize(signal, signal_size, lb, ub);

  /********************************
  Start Detrend
  *********************************/
  getDetrend(result, signal_size);

  /********************************
  Start wavelet
  *********************************/
  obj = wave_init(name);  // Initialize the wavelet
  inp = (double*)malloc(sizeof(double) * signal_size);
  for (int i = 0; i < signal_size; ++i) {
    inp[i] = result[i];
  }
  wt = wt_init(obj, "dwt", signal_size,
               wlvl);  // Initialize the wavelet transform object
  dwt(wt, inp);        // Perform DWT
  // ESP_LOGI("SWU_BME wavelet", "%d", wt->length[0]);
  free(inp);
  return wt->output;
}

void predict(double* my_input) {
  // ESP_LOGI("SWU_BME", "start predict");
  int my_result = -1;
  float my_result_acc = 0;
  size_t input_size = 195;
  for (size_t i = 0; i < input_size; i++) {
    ESP_LOGI("SWU_BME", "pre process %f" ,my_input[i] );
    input->data.f[i] = my_input[i];
  }

  // Run inference, and report any error
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed ");
    return;
  }

  // Read the predicted y value from the model's output tensor

  int numElements = output->dims->data[1];
  for (int i = 0; i < numElements; i++) {
    ESP_LOGI("SWU_BME", "data %d : %f", i,output->data.f[i]);
    //&& output->data.f[i] > 0.70
    if(output->data.f[i] > my_result_acc ){
      my_result_acc = output->data.f[i];
      my_result = i;
    }
    // HandleOutput(error_reporter, x_val, output->data.f[i], i);
    // ESP_LOGI("SWU_BME", "expected : %f , result : %d", output->data.f[i], i);
  }
  // ESP_LOGI("SWU_BME", "%d", my_result);
  // ESP_LOGI("SWU_BME", "%f", my_result_acc);
  result = std::to_string(my_result);
  result_acc = std::to_string(my_result_acc);
}

static void http_data_task(void* pvParameters) {
  while (1) {
    if (!loopEcg) {
      // ESP_LOGI("SWU_BME", "http_test_task %d", signal_inx);
      // ESP_LOGI("SWU_BME", "http_test_task %s", str.c_str());
      str = "{\"data\":\"" + str + "\",\"predict\":\"" + result + "\",\"accuracy\":\"" + result_acc + "\",\"create_by\":\"" + create_by +
            "\",\"edit_by\":\"" + edit_by + "\"}";
      http_rest_with_url(str.c_str());
      str = "";
      loopEcg = true;
    }
  }
}

// The name of this function is important for Arduino compatibility.
void setup() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);

  // Keep track of how many inferences we have performed.
  inference_count = 0;

  // my setup
  nvs_flash_init();
  max30003_initchip(PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SCK, PIN_SPI_CS);
  wifi_init();
  ESP_LOGI("SWU_BME", "nvs_flash_init");

  connectWifi();
  // double signal_arr[] = {0,0,0,0,0,0,0,0,0,0};
  // preProcess(signal_arr);
  xTaskCreate(&http_data_task, "http_data_task", 8192, NULL, 5, NULL);
}

// The name of this function is important for Arduino compatibility.
void loop() {
  if (loopEcg) {
    long db = max30003_read_ecg_data();
    // double mv = db / (3.3 * 262144);
    // ESP_LOGI("SWU_BME", "%ld", db);
    // ESP_LOGI("SWU_BME", "%f", mv);

    if (signal_inx < signal_target) {
      signal_arr[signal_inx] = db;
      signal_inx++;
      // ESP_LOGI("SWU_BME", "signal_inx , %d , mv , %f", signal_inx, mv);

      str = str + "," + std::to_string(db);
    } else if (signal_inx == signal_target) {

      signal_inx = 0;

      double* input = preProcess(signal_arr);

      predict(input);
      // ESP_LOGI("SWU_BME", "%s", result.c_str());
      // ESP_LOGI("SWU_BME", "%s", result_acc.c_str());
      // loopEcg = false;
      // ESP_LOGI("SWU_BME", " loop end ");
    }
    vTaskDelay(12 / portTICK_PERIOD_MS);
  }
}
