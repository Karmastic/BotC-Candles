#pragma once

#ifdef ESP32_DEVKIT

#define SERIAL_DEBUG_BAUDRATE 115200
#define ACTIVITY_LED 2
#define RESET_CONFIG_INPUT 14
#define RESET_CONFIG_INPUT_MODE INPUT
#define RESET_CONFIG_INPUT_STATE LOW

#elif defined ESP32_32E // LCD/Touchscreen

#define SERIAL_DEBUG_BAUDRATE 115200
#define ACTIVITY_LED 17
#define RESET_CONFIG_INPUT 35
#define RESET_CONFIG_INPUT_MODE INPUT_PULLDOWN
#define RESET_CONFIG_INPUT_STATE HIGH

#else // Not a known platform config?

#pragma error "Unknown Platform.io configuration selected"

#endif