#pragma once
#include <Adafruit_ADS1X15.h>
#include <vector>

#define WIFI_SSID "Kembar Dua"
#define WIFI_PASS "sasa1234"

#define MQTT_BROKER "192.168.1.11"
#define MQTT_USER "esp_user"
#define MQTT_PASS "5127"
#define MQTT_PORT 1883

// Main Tank Config
#define MT_US_TRIG_PIN 25 // GPIO
#define MT_US_ECHO_PIN 26 // GPIO
#define MT_VALVE_PIN 4    // MCP
#define MT_MIXER_PIN 5    // MCP

// Measuring Tank Config
#define MeT_PH_PIN ADS1X15_REG_CONFIG_MUX_SINGLE_0  // ADS
#define MeT_TDS_PIN ADS1X15_REG_CONFIG_MUX_SINGLE_1 // ADS
#define MeT_TEMP_PIN 4                              // GPIO
#define MeT_VALVE_PIN 7                             // MCP
#define MeT_PUMP_PIN 6                              // MCP

// pH Corrector Config
#define PH_UP_US_TRIG_PIN 15   // GPIO
#define PH_UP_US_ECHO_PIN 2    // GPIO
#define PH_DOWN_US_TRIG_PIN 16 // GPIO
#define PH_DOWN_US_ECHO_PIN 17 // GPIO
#define PH_UP_PUMP_PIN 2       // MCP
#define PH_DOWN_PUMP_PIN 3     // MCP

// TDS Corrector Config
#define TDS_A_US_TRIG_PIN 27 // GPIO
#define TDS_A_US_ECHO_PIN 14 // GPIO
#define TDS_B_US_TRIG_PIN 12 // GPIO
#define TDS_B_US_ECHO_PIN 13 // GPIO
#define TDS_TDS_PUMP_PIN 1   // MCP

// Physical Control Config
#define PC_BUZZER_PIN 0             // MCP
#define PC_LED_HEARTBEAT_PIN 8      // MCP
#define PC_LED_WARNING_PIN 9        // MCP
#define PC_LED_WIFI_PIN 10          // MCP
#define PC_LED_MQTT_PIN 11          // MCP
#define PC_LED_READ_ENV_PIN 12      // MCP
#define PC_LED_ADD_EVENT_PIN 13     // MCP
#define PC_BTN_RECONNECT_PIN 34     // GPIO
#define PC_BTN_READ_ENV_PIN 35      // GPIO
#define PC_BTN_ADD_PH_UP_PIN 36     // GPIO
#define PC_BTN_ADD_PH_DOWN_PIN 39   // GPIO
#define PC_BTN_ADD_NUTRITION_PIN 32 // GPIO