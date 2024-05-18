/**
Left Slide Controller
Description: This sketch controls the 2 lights and sensors in the left slide.
Author: Ron Lisle

D19 (A0) PIR LivingRoomMotion
D13 (A2) Light Couch
D14 (A5) Light LeftVertical
 */

#include <IoT.h>

#define LIVINGROOM_MOTION_TIMEOUT_MSECS 15*1000

//TODO: change to const
#define CONTROLLER_NAME "LeftSlide"
#define MQTT_BROKER "192.168.0.33"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(AUTOMATIC);

int const livingRoomMotionTimeoutSecs = 3*60;

// Write Log messages to MQTT and/or serial port
#define MQTT_LOGGING true
// View logs with CLI using 'particle serial monitor --follow'
//SerialLogHandler logHandler1(57600, LOG_LEVEL_ALL);

// Timing
bool isTimingLivingRoomMotion;

void loop() {
    IoT::loop();
}

void setup() {

//    WiFi.setCredentials(WIFI_SSID, WIFI_PASSWORD);
//    WiFi.selectAntenna(ANT_INTERNAL);    
//    WiFi.useDynamicIP();

    IoT::begin(MQTT_BROKER, CONTROLLER_NAME, MQTT_LOGGING);

    // Create Devices
    // Sensors
    Device::add(new PIR(D19, "LivingRoomMotion", "Living Room", livingRoomMotionTimeoutSecs));

    // Lights (default 2s curve 2)
    Device::add(new Light(A2, "Couch", "Living Room"));
    Device::add(new Light(A5, "LeftVertical", "Living Room"));
}
