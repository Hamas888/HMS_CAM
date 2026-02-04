/*
 ============================================================================================================================================
 * File:        HMS_CAM.h
 * Author:      Hamas Saeed
 * Version:     Rev_1.0.0
 * Date:        Jan 28 2026
 * Brief:       This file package provides camera driver for embedded systems.
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

#ifndef HMS_CAM_H
#define HMS_CAM_H

#include "HMS_CAM_Config.h"

class HMS_CAM {
public:
    HMS_CAM();
    ~HMS_CAM();
    
    HMS_CAM_StatusTypeDef stop();
    HMS_CAM_StatusTypeDef begin();
    HMS_CAM_StatusTypeDef flush();
    HMS_CAM_StatusTypeDef refresh();
    HMS_CAM_StatusTypeDef captureFrame(HMS_CAM_FrameBufferTypeDef &frame);

    #ifdef HMS_CAM_PLATFORM_ESP_IDF
        void returnFrameBuffer();

        void setFrameSize(framesize_t size)                 { _frameSize = size;      }
        void setPixelFormat(pixformat_t format)             { _pixelFormat = format;  }
        void setGrabMode(camera_grab_mode_t mode)           { _grabMode = mode;       }
        void setFBLocation(camera_fb_location_t location)   { _fbLocation = location; }

        camera_fb_t* getFrameBuffer()                       { return _fb;             }
    #elif defined(HMS_CAM_PLATFORM_ARDUINO)
        void setFrameSize(int size)                         { _frameSize = size;      }
        void setPixelFormat(int format)                     { _pixelFormat = format;  }
        void setGrabMode(int mode)                          { _grabMode = mode;       }
        void setFBLocation(int location)                    { _fbLocation = location; }
    #endif

    void setFBCount(int count)                              { _fbCount = count;       }
    void setJPEGQuality(int quality)                        { _jpegQuality = quality; }
    void setXCLKFrequency(int freqHz)                       { _frequencyHz = freqHz;  }

private:
    #ifdef HMS_CAM_PLATFORM_ESP_IDF
        camera_fb_t             *_fb            = NULL;                                     // Frame buffer pointer ESP-IDF
        framesize_t             _frameSize      = FRAMESIZE_QQVGA;                          // Default to QQVGA ESP-IDF
        pixformat_t             _pixelFormat    = PIXFORMAT_JPEG;                           // Default to JPEG  ESP-IDF
        camera_grab_mode_t      _grabMode       = CAMERA_GRAB_WHEN_EMPTY;                   // Default grab mode ESP-IDF
        camera_fb_location_t    _fbLocation     = CAMERA_FB_IN_DRAM;                        // Default to DRAM  ESP-IDF
    #elif defined(HMS_CAM_PLATFORM_ARDUINO)
        uint8_t                 *_fb            = NULL;                                     // Frame buffer pointer Arduino
        int                     _frameSize      = FRAMESIZE_QQVGA;                          // Default to QQVGA Arduino
        int                     _pixelFormat    = PIXFORMAT_JPEG;                           // Default to JPEG Arduino
        int                     _grabMode       = CAMERA_GRAB_WHEN_EMPTY;                   // Default grab mode Arduino
        int                     _fbLocation     = CAMERA_FB_IN_DRAM;                        // Default to DRAM Arduino
    #endif

    int                         _jpegQuality    = 20;                                       // JPEG quality (0-63), lower means better quality
    int                         _frequencyHz    = 20000000;                                 // XCLK frequency in Hz
    bool                        _initialized    = false;                                    // Initialization state
    size_t                      _fbCount        = 1;                                        // Size of the allocated buffer

    HMS_CAM_StatusTypeDef _initCamera();
    HMS_CAM_StatusTypeDef _configureSensor();
    HMS_CAM_StatusTypeDef _refreshSettings();
    HMS_CAM_StatusTypeDef _verifyConnections();
};

#endif // HMS_CAM_H