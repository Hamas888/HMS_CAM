# HMS_CAM/CMakeLists.txt

set(HMS_CAM_VERSION 1.0.0)

# Check if we're building with Zephyr
if(DEFINED ZEPHYR_BASE)
    zephyr_library_sources(src/HMS_CAM.cpp)
    zephyr_include_directories(include)

# Check if we're building with ESP-IDF
elseif(DEFINED ESP_PLATFORM OR DEFINED IDF_VER OR DEFINED ENV{IDF_PATH})
    idf_component_register(
        INCLUDE_DIRS "include"
        SRCS 
            "src/HMS_CAM.cpp"
            "src/HMS_CAM_ESP32.cpp"
        REQUIRES
            "driver"
            "esp_timer"
            "freertos"
            "esp_common"
            "esp32-camera"
    )

    # Force C++17 standard
    target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)

    # (optional) if you want strict enforcement instead of compiler default fallback
    set_target_properties(${COMPONENT_LIB} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    
# STM32 / generic CMake project
else()
    add_library(HMS_CAM INTERFACE)
    target_include_directories(HMS_CAM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_features(HMS_CAM INTERFACE cxx_std_17)
endif()