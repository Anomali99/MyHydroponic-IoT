#pragma once

#define WIFI_SSID "NamaSSIDAnda"
#define WIFI_PASS "PasswordAnda"

#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-hidroponik"

// Main Tank Pin
#define MT_US_TRIG_PIN 12
#define MT_US_ECHO_PIN 12
#define MT_VALVE_PIN 12
#define MT_MIXER_PIN 12

// Measuring Tank Pin
#define MeT_PH_PIN 12
#define MeT_TDS_PIN 12
#define MeT_VALVE_PIN 12
#define MeT_PUMP_PIN 12

// pH Corrector Pin
#define PH_UP_US_TRIG_PIN 12
#define PH_UP_US_ECHO_PIN 12
#define PH_DOWN_US_TRIG_PIN 12
#define PH_DOWN_US_ECHO_PIN 12
#define PH_UP_PUMP_PIN 12
#define PH_DOWN_PUMP_PIN 12

// TDS Corrector Pin
#define TDS_A_US_TRIG_PIN 12
#define TDS_A_US_ECHO_PIN 12
#define TDS_B_US_TRIG_PIN 12
#define TDS_B_US_ECHO_PIN 12
#define TDS_TDS_PUMP_PIN 12

// Physical Control Pin
#define PC_BUZZER_PIN 12
#define PC_LED_WIFI_PIN 12
#define PC_LED_MQTT_PIN 12
#define PC_LED_INTERNET_PIN 12
#define PC_LED_READ_ENV_PIN 12
#define PC_LED_ADD_EVVENT_PIN 12
#define PC_BTN_RECONNECT_PIN 12
#define PC_BTN_READ_ENH_PIN 12
#define PC_BTN_ADD_PH_UP_PIN 12
#define PC_BTN_ADD_PH_DOWN_PIN 12
#define PC_BTN_ADD_NUTRITION_PIN 12