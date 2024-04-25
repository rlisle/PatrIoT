/**
  FrontPanel Controller
  Description: This sketch controls all the switches in the Cyclone 4005 front control panel.
  Author: Ron Lisle
  Date: 9/16/17
 */
#include <IoT.h>

#define CONTROLLER_NAME "FrontPanel"
#define MQTT_BROKER "192.168.0.33"
#define PCA9685_ADDRESS 0x41       // Lowest jumper set

int const frontDoorLightTimeout = 15*1000;
int const livingRoomMotionTimeout = 3*1000;
int const coffeeMotionTimeout = 3*1000;

#define CURVE 2 // 0 = Linear, 1 = exponential, 2 = 50/50

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(AUTOMATIC);

// Write Log messages to MQTT and/or serial port
#define MQTT_LOGGING true
//SerialLogHandler logHandler1(57600, LOG_LEVEL_INFO);

// Global Variables
int voltage = 0;
bool isTimingLivingRoomMotion = false;
bool isTimingFrontDoor = false;
bool isTimingCoffeeMotion = false;


// LOOP
void loop() {
    IoT::loop();
}


// SETUP
void setup() {
//    WiFi.setCredentials(WIFI_SSID, WIFI_PASSWORD);
//    WiFi.selectAntenna(ANT_INTERNAL);
//    WiFi.useDynamicIP();
    
    // This also should set timezone and DST
    IoT::begin(MQTT_BROKER, CONTROLLER_NAME, MQTT_LOGGING);

    // Required by NCD8Light
    PCA9685::initialize(PCA9685_ADDRESS);

    // Inside Lights    TODO: set actual light #s     TODO: set wire colors
    Device::add(new NCD16Light(15, "KitchenCeiling", "Kitchen",0));   // R
    Device::add(new NCD16Light(13, "Sink", "Kitchen",0));             // B
    Device::add(new NCD16Light(12, "LeftTrim", "Kitchen",0));         // G?
    Device::add(new NCD16Light(16, "RightTrim", "Living Room",0));    // Y?
    Device::add(new NCD16Light(3,  "Ceiling", "Living Room",0));      // W
    Device::add(new NCD16Light(14, "CabinetLights", "Kitchen",0));     // R

    // Zigbee Lights
    Device::add(new ZigbeeLight("NookLamp", "Nook"));
    Device::add(new ZigbeeLight("BedroomLamp", "Bedroom"));

    // Zigbee Outlets
    Device::add(new ZigbeeLight("PlantLight", "Kitchen"));

    // Outside Lights
    Device::add(new NCD16Light(1, "DoorSide", "Outside",0));          // B
    Device::add(new NCD16Light(5, "OtherSide", "Outside",0));         // G
    Device::add(new NCD16Light(2, "FrontAwning", "Outside",0));       // Y
    Device::add(new NCD16Light(4, "FrontPorch", "Outside",0));        // W

    // Zigbee Motion Detectors & Contact Switches
    Device::add(new ZigbeeMotion("CoffeeMotion", "Office"));
    Device::add(new ZigbeeContact("FrontDoor", "Living Room"));

    // 12V Monitor (actually 14.27) with 10:1 R-Ladder
    // Adjust fullScale to reflect actual R-Ladder (36.9)
//    Device::add(new Voltage(A0, "FP volts", "LivingRoom", 36.9, 10));
    
//    Device::add(new Power("Power", "Status"));

}
