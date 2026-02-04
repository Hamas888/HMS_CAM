#include "HMS_CAM.h"

#if HMS_CAM_DEBUG_ENABLED
    ChronoLogger *camLogger = nullptr;
#endif

HMS_CAM::HMS_CAM() {
    #if HMS_CAM_DEBUG_ENABLED
        if (!camLogger) {
            camLogger = new ChronoLogger("HMS_CAM");
            camLogger->setLevel(HMS_CAM_LOG_LEVEL);
        }
    #endif
    // Constructor implementation
}

HMS_CAM::~HMS_CAM() {
    #ifdef HMS_CAM_PLATFORM_ESP_IDF
        returnFrameBuffer();
    #endif

    #if HMS_CAM_DEBUG_ENABLED
        if (camLogger) {
            delete camLogger;
            camLogger = nullptr;
        }
    #endif
}

HMS_CAM_StatusTypeDef HMS_CAM::begin() {
    HMS_CAM_LOGGER(info, "Initializing HMS CAM...");

    HMS_CAM_StatusTypeDef status = _initCamera();
    if (status != HMS_CAM_OK) {
        HMS_CAM_LOGGER(error, "Camera initialization failed");
        return status;
    }

    status = _configureSensor();
    if (status != HMS_CAM_OK) {
        HMS_CAM_LOGGER(error, "Sensor configuration failed");
        return status;
    }

    HMS_CAM_LOGGER(info, "HMS CAM initialized successfully");
    _initialized = true;
    return HMS_CAM_OK;
}

HMS_CAM_StatusTypeDef HMS_CAM::stop() {
    HMS_CAM_LOGGER(info, "Stopping HMS CAM...");
    #ifdef HMS_CAM_PLATFORM_ESP_IDF
        returnFrameBuffer();
        esp_err_t err = esp_camera_deinit();
        if (err != ESP_OK) {
            HMS_CAM_LOGGER(error, "Camera de-initialization failed: %s", esp_err_to_name(err));
            return HMS_CAM_ERROR;
        }
    #endif
    _initialized = false;
    return HMS_CAM_OK;
}

HMS_CAM_StatusTypeDef HMS_CAM::refresh() {
    if (!_initialized) {
        HMS_CAM_LOGGER(error, "Camera not initialized. Call begin() first.");
        return HMS_CAM_ERROR;
    }

    HMS_CAM_LOGGER(info, "Refreshing camera settings...");

    HMS_CAM_StatusTypeDef status = _refreshSettings();
    if (status != HMS_CAM_OK) {
        HMS_CAM_LOGGER(error, "Failed to refresh camera settings");
        return status;
    }

    HMS_CAM_LOGGER(info, "Camera settings refreshed successfully");
    return HMS_CAM_OK;
}
