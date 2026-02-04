/*
 ============================================================================================================================================
 * File:        HMS_CAM_Config.h
 * Author:      Hamas Saeed
 * Version:     Rev_1.0.0
 * Date:        Jan 28 2026
 * Brief:       This file package provides camera driver configuration options.
 ============================================================================================================================================
 * License: 
 * MIT License
 * 
 * Copyright (c) 2025 Hamas Saeed
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do 
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For any inquiries, contact Hamas Saeed at hamasaeed@gmail.com
 ============================================================================================================================================
 */

#ifndef HMS_CAM_CONFIG_H
#define HMS_CAM_CONFIG_H

/*
  ┌─────────────────────────────────────────────────────────────────────┐
  │ Note:     Platform detection                                        │
  │ Requires: Development environment and platform-specific SDKs        │
  └─────────────────────────────────────────────────────────────────────┘
*/
#if defined(ARDUINO)
  #include <Arduino.h>
  #if defined(ESP32)
    #define HMS_CAM_ARDUINO_ESP32
  #elif defined(ESP8266)
    // Include ESP8266 MQTT libraries here
    #define HMS_CAM_ARDUINO_ESP8266
  #endif
  #define HMS_CAM_PLATFORM_ARDUINO
#elif defined(ESP_PLATFORM)
  #include <stdio.h>
  #include <stdint.h>
  #include "esp_timer.h"
  #include "esp_err.h"
  #include "driver/gpio.h"
  #include "esp_camera.h"
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #define HMS_CAM_PLATFORM_ESP_IDF
#elif defined(__ZEPHYR__)
  #define HMS_CAM_PLATFORM_ZEPHYR
#elif defined(__STM32__)
  #define HMS_CAM_PLATFORM_STM32_HAL
#elif defined(__linux__) || defined(_WIN32) || defined(__APPLE__)
  // Desktop specific includes
  #define HMS_CAM_PLATFORM_DESKTOP
#endif // Platform detection

/*
  ┌─────────────────────────────────────────────────────────────────────┐
  │ Note:     Enable only if ChronoLog is included                      │
  │ Requires: ChronoLog library → https://github.com/Hamas888/ChronoLog │
  └─────────────────────────────────────────────────────────────────────┘
*/
#if defined(CONFIG_HMS_CAM_DEBUG)
    #define HMS_CAM_DEBUG_ENABLED              1
#elif defined(HMS_CAM_DEBUG)
    #define HMS_CAM_DEBUG_ENABLED              1
#else
    #define HMS_CAM_DEBUG_ENABLED              0                            // Enable debug messages (1=enabled, 0=disabled)
#endif


#if HMS_CAM_DEBUG_ENABLED
  #if __has_include("ChronoLog.h")
    #include "ChronoLog.h"
    #ifndef HMS_CAM_LOG_LEVEL
        #ifdef CONFIG_HMS_CAM_LOG_LEVEL
            #if CONFIG_HMS_CAM_LOG_LEVEL == 0
                #define HMS_CAM_LOG_LEVEL      CHRONOLOG_LEVEL_ERROR
            #elif CONFIG_HMS_CAM_LOG_LEVEL == 1
                #define HMS_CAM_LOG_LEVEL      CHRONOLOG_LEVEL_WARN
            #elif CONFIG_HMS_CAM_LOG_LEVEL == 2
                #define HMS_CAM_LOG_LEVEL      CHRONOLOG_LEVEL_INFO
            #elif CONFIG_HMS_CAM_LOG_LEVEL == 3
                #define HMS_CAM_LOG_LEVEL      CHRONOLOG_LEVEL_DEBUG
            #elif CONFIG_HMS_CAM_LOG_LEVEL == 4
                #define HMS_CAM_LOG_LEVEL      CHRONOLOG_LEVEL_DEBUG
            #else
                #define HMS_CAM_LOG_LEVEL      CHRONOLOG_LEVEL_DEBUG
            #endif
        #else
            #define HMS_CAM_LOG_LEVEL          CHRONOLOG_LEVEL_DEBUG
        #endif
      extern ChronoLogger *camLogger;
    #endif
    #define HMS_CAM_LOGGER(level, msg, ...)    \
      do {                                     \
        if (camLogger)                         \
          camLogger->level(                    \
            msg, ##__VA_ARGS__                 \
          );                                   \
      } while (0)
  #else
    #error "ChronoLog library not found. Please include ChronoLog to enable HMS_CAM debugging."
  #endif
#else
  #define HMS_CAM_LOGGER(level, msg, ...)       do {} while (0)
#endif

/*
  ┌─────────────────────────────────────────────────────────────────────┐
  │ Note: HMS CAM Custom Types & Definitions                            │
  └─────────────────────────────────────────────────────────────────────┘
*/

#define HMS_CAM_Y9_GPIO_NUM                     CONFIG_HMS_CAM_Y9_GPIO
#define HMS_CAM_Y8_GPIO_NUM                     CONFIG_HMS_CAM_Y8_GPIO
#define HMS_CAM_Y7_GPIO_NUM                     CONFIG_HMS_CAM_Y7_GPIO
#define HMS_CAM_Y6_GPIO_NUM                     CONFIG_HMS_CAM_Y6_GPIO
#define HMS_CAM_Y5_GPIO_NUM                     CONFIG_HMS_CAM_Y5_GPIO
#define HMS_CAM_Y4_GPIO_NUM                     CONFIG_HMS_CAM_Y4_GPIO
#define HMS_CAM_Y3_GPIO_NUM                     CONFIG_HMS_CAM_Y3_GPIO
#define HMS_CAM_Y2_GPIO_NUM                     CONFIG_HMS_CAM_Y2_GPIO
#define HMS_CAM_LED_GPIO_NUM                    CONFIG_HMS_CAM_LED_GPIO
#define HMS_CAM_XCLK_GPIO_NUM                   CONFIG_HMS_CAM_XCLK_GPIO
#define HMS_CAM_SIOD_GPIO_NUM                   CONFIG_HMS_CAM_SIOD_GPIO
#define HMS_CAM_SIOC_GPIO_NUM                   CONFIG_HMS_CAM_SIOC_GPIO
#define HMS_CAM_PWDN_GPIO_NUM                   CONFIG_HMS_CAM_PWDN_GPIO
#define HMS_CAM_HREF_GPIO_NUM                   CONFIG_HMS_CAM_HREF_GPIO
#define HMS_CAM_PCLK_GPIO_NUM                   CONFIG_HMS_CAM_PCLK_GPIO
#define HMS_CAM_VSYNC_GPIO_NUM                  CONFIG_HMS_CAM_VSYNC_GPIO
#define HMS_CAM_RESET_GPIO_NUM                  CONFIG_HMS_CAM_RESET_GPIO


#define HMS_CAM_ERROR_CHECK(expr)               \
  do {                                          \
    if (!(expr)) {                              \
      HMS_CAM_LOGGER(error, "Error: %s", #expr);\
      return HMS_CAM_ERROR;                     \
    }                                           \
  } while (0)

#ifdef HMS_CAM_PLATFORM_ESP_IDF
  #define HMS_CAM_Delay(ms) vTaskDelay(pdMS_TO_TICKS(ms))
#elif defined(HMS_CAM_PLATFORM_ARDUINO)
  #define HMS_CAM_Delay(ms) delay(ms)
#else
  #define HMS_CAM_Delay(ms)
#endif

typedef enum {
  HMS_CAM_OK                                    = 0x00,
  HMS_CAM_BUSY                                  = 0x01,
  HMS_CAM_ERROR                                 = 0x02,
  HMS_CAM_NO_MEM                                = 0x03,
  HMS_CAM_TIMEOUT                               = 0x04,
  HMS_CAM_NOT_FOUND                             = 0x05,
} HMS_CAM_StatusTypeDef;

typedef enum {
  HMS_CAM_OV2640                                = 0x00,
  HMS_CAM_OV3660                                = 0x01,
  HMS_CAM_OV5640                                = 0x02,
  HMS_CAM_OV7670                                = 0x03,
} HMS_CAM_ModuleType;

typedef struct {
  uint8_t *buf;                                                             // Pointer to the pixel data
  size_t length;                                                            // Length of the buffer in bytes
  size_t width;                                                             // Width of the buffer in pixels
  size_t height;                                                            // Height of the buffer in pixels
} HMS_CAM_FrameBufferTypeDef;

#endif // HMS_CAM_CONFIG_H