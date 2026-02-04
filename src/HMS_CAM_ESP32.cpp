#include "HMS_CAM.h"

#ifdef HMS_CAM_PLATFORM_ESP_IDF 

void HMS_CAM::returnFrameBuffer() {
    if (_fb != NULL) {
        esp_camera_fb_return(_fb);
        _fb = NULL;
    }
}

HMS_CAM_StatusTypeDef HMS_CAM::flush() {
    returnFrameBuffer();
    return HMS_CAM_OK;
}

HMS_CAM_StatusTypeDef HMS_CAM::_initCamera() {
    HMS_CAM_LOGGER(info, "Initializing camera...");
    _verifyConnections();

    camera_config_t config;

    config.ledc_channel     = LEDC_CHANNEL_0;
    config.ledc_timer       = LEDC_TIMER_0;
    config.pin_d0           = HMS_CAM_Y2_GPIO_NUM;
    config.pin_d1           = HMS_CAM_Y3_GPIO_NUM;
    config.pin_d2           = HMS_CAM_Y4_GPIO_NUM;
    config.pin_d3           = HMS_CAM_Y5_GPIO_NUM;
    config.pin_d4           = HMS_CAM_Y6_GPIO_NUM;
    config.pin_d5           = HMS_CAM_Y7_GPIO_NUM;
    config.pin_d6           = HMS_CAM_Y8_GPIO_NUM;
    config.pin_d7           = HMS_CAM_Y9_GPIO_NUM;
    config.pin_xclk         = HMS_CAM_XCLK_GPIO_NUM;
    config.pin_pclk         = HMS_CAM_PCLK_GPIO_NUM;
    config.pin_vsync        = HMS_CAM_VSYNC_GPIO_NUM;
    config.pin_href         = HMS_CAM_HREF_GPIO_NUM;
    config.pin_sccb_sda     = HMS_CAM_SIOD_GPIO_NUM;
    config.pin_sccb_scl     = HMS_CAM_SIOC_GPIO_NUM;
    config.pin_pwdn         = HMS_CAM_PWDN_GPIO_NUM;
    config.pin_reset        = HMS_CAM_RESET_GPIO_NUM;
    config.xclk_freq_hz     = _frequencyHz;
    config.pixel_format     = _pixelFormat;
    config.frame_size       = _frameSize;
    config.jpeg_quality     = _jpegQuality;
    config.fb_count         = _fbCount; 
    config.fb_location      = _fbLocation;
    config.grab_mode        = _grabMode;

    esp_err_t err = esp_camera_init(&config);                                           // First attempt with current JPEG settings
    
    if (err != ESP_OK) {                                                                // If JPEG fails, try with smaller frame size first
        HMS_CAM_LOGGER(warn, "Camera init failed with %s, trying QQVGA...", esp_err_to_name(err));
        config.frame_size = FRAMESIZE_QQVGA;
        err = esp_camera_init(&config);
    }

    if (err != ESP_OK) {
        HMS_CAM_LOGGER(error, "Camera initialization failed: %s", esp_err_to_name(err));
        return HMS_CAM_ERROR;
    }

    return HMS_CAM_OK;
}

HMS_CAM_StatusTypeDef HMS_CAM::_configureSensor() {
    sensor_t * s = esp_camera_sensor_get();
    if (s != NULL) {
        s->set_brightness(s, 0);                                                        // -2 to 2
        s->set_contrast(s, 0);                                                          // -2 to 2
        s->set_saturation(s, 0);                                                        // -2 to 2
        s->set_special_effect(s, 0);                                                    // 0 - No Effect
        s->set_whitebal(s, 1);                                                          // Enable white balance
        s->set_awb_gain(s, 1);                                                          // Enable AWB gain
        s->set_wb_mode(s, 0);                                                           // Auto white balance
        s->set_exposure_ctrl(s, 1);                                                     // Enable exposure control
        s->set_aec2(s, 0);                                                              // Disable AEC2
        s->set_ae_level(s, 0);                                                          // Auto exposure level
        s->set_aec_value(s, 300);                                                       // AEC value
        s->set_gain_ctrl(s, 1);                                                         // Enable gain control
        s->set_agc_gain(s, 0);                                                          // Auto gain
        s->set_gainceiling(s, (gainceiling_t)0);                                        // Gain ceiling
        s->set_bpc(s, 1);                                                               // Enable black pixel correct
        s->set_wpc(s, 1);                                                               // Enable white pixel correct
        s->set_raw_gma(s, 1);                                                           // Enable gamma
        s->set_lenc(s, 1);                                                              // Enable lens correction
        s->set_hmirror(s, 0);                                                           // No horizontal mirror
        s->set_vflip(s, 0);                                                             // No vertical flip
        s->set_dcw(s, 1);                                                               // Enable downsize
        s->set_colorbar(s, 0);                                                          // Disable color bar

        HMS_CAM_Delay(500);      
        HMS_CAM_LOGGER(debug, "Camera sensor configured");
        return HMS_CAM_OK;
    }
    return HMS_CAM_ERROR;
}

HMS_CAM_StatusTypeDef HMS_CAM::_refreshSettings() {
    HMS_CAM_LOGGER(info, "Refreshing camera settings...");

    _initialized = false;
    esp_camera_deinit();

    HMS_CAM_StatusTypeDef status = _initCamera();
    if (status != HMS_CAM_OK) {
        HMS_CAM_LOGGER(error, "Camera re-initialization failed");
        return status;
    } else {
        HMS_CAM_LOGGER(info, "Camera re-initialized successfully");
       _initialized = true;
    }

    return HMS_CAM_OK;

}

HMS_CAM_StatusTypeDef HMS_CAM::_verifyConnections() {
    HMS_CAM_LOGGER(debug, "Verifying camera GPIO connections...");
    
    gpio_set_direction((gpio_num_t)HMS_CAM_XCLK_GPIO_NUM, GPIO_MODE_OUTPUT);                    // Verify XCLK output
    gpio_set_level((gpio_num_t)HMS_CAM_XCLK_GPIO_NUM, 1);

    HMS_CAM_Delay(10);

    gpio_set_level((gpio_num_t)HMS_CAM_XCLK_GPIO_NUM, 0);
    
    gpio_set_direction((gpio_num_t)HMS_CAM_SIOD_GPIO_NUM, GPIO_MODE_INPUT_OUTPUT_OD);           // Verify I2C lines
    gpio_set_direction((gpio_num_t)HMS_CAM_SIOC_GPIO_NUM, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode((gpio_num_t)HMS_CAM_SIOD_GPIO_NUM, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)HMS_CAM_SIOC_GPIO_NUM, GPIO_PULLUP_ONLY);
    
    HMS_CAM_LOGGER(
        debug, "CAM SDA level: %d, CAM SCL level: %d", 
        gpio_get_level((gpio_num_t)HMS_CAM_SIOD_GPIO_NUM),
        gpio_get_level((gpio_num_t)HMS_CAM_SIOC_GPIO_NUM)
    );

    gpio_config_t log_conf = {};

    log_conf.mode           = GPIO_MODE_OUTPUT;
    log_conf.pin_bit_mask   = (1ULL << HMS_CAM_LED_GPIO_NUM);

    gpio_config(&log_conf);
    gpio_set_level((gpio_num_t)HMS_CAM_LED_GPIO_NUM, 0);

    return HMS_CAM_OK;
}

HMS_CAM_StatusTypeDef HMS_CAM::captureFrame(HMS_CAM_FrameBufferTypeDef &frame) {
    if(!_initialized) {
        HMS_CAM_LOGGER(error, "Camera not initialized. Call begin() first.");
        return HMS_CAM_ERROR;
    }

    frame.buf    = nullptr;
    frame.length = 0;
    frame.width  = 0;
    frame.height = 0;

    // Retry loop for valid frame capture
    for (int retry = 0; retry < 3; retry++) {
        if(_fb) {
            esp_camera_fb_return(_fb);
            _fb = NULL;
        }
        
        _fb = esp_camera_fb_get();
        if (!_fb) {
            HMS_CAM_LOGGER(warn, "Failed to capture frame, retry %d...", retry + 1);
            HMS_CAM_Delay(50);
            continue;
        }

        // Check for valid JPEG header if in JPEG mode
        if (_pixelFormat == PIXFORMAT_JPEG) {
            if (_fb->len < 100 || _fb->buf[0] != 0xFF || _fb->buf[1] != 0xD8) {
                HMS_CAM_LOGGER(warn, "Invalid JPEG frame detected, retrying...");
                continue;
            }
        }

        // If we reach here, we have a valid frame or we're not in JPEG mode
        frame.buf    = _fb->buf;
        frame.length = _fb->len;
        frame.width  = _fb->width;
        frame.height = _fb->height;

        HMS_CAM_LOGGER(debug, "Frame captured: %ux%u, size: %u bytes", frame.width, frame.height, frame.length);
        return HMS_CAM_OK;
    }

    HMS_CAM_LOGGER(error, "Failed to capture valid frame after retries");
    return HMS_CAM_ERROR;
}

#endif // HMS_CAM_PLATFORM_ESP_IDF