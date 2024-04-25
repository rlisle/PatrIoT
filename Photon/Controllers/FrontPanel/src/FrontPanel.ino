/**
  FrontPanel Controller
  Description: This sketch controls all the switches in the Cyclone 4005 front control panel.
  Author: Ron Lisle
  Date: 9/16/17
  
  To update Photon:
   1. Edit this code
   2. Update IoT and plugins if needed
   USB:
   3. Put Photon into listen mode using buttons
   4. "particle flash --usb <binname>"
   OTA:
   4. "particle flash front_panel2 --target 5.6.0" or "ffp"

  NCD 16x Dimmer
  1. DS Flood Lights
  2. Awning Light
  3. Ceiling
  4. Porch Lights
  5. ODS Flood Lights
  6.
  7.
  8.
  9.
  10.
  11.
  12. Indirect
  13. Cabinet
  14. Sink
  15. Ceiling
  16. ?
 
  PHOTON 2 PINS (in order on card)
  Left side
  RST - nc
  3V3 - nc
  Mode - nc
  Gnd - nc              - 12v monitor R ladder gnd
  D11/A0/ADC4           - 12v monitor R ladder
  D12/A1/ADC5
  D13/A2/ADC3/PWM
  D14/A5/ADC0/PWM
  D19/S4                - Front Door switch
  D18/S3
  D17/SCK
  D15/MOSI/PWM          - Ceiling LEDs
  D16/MISO/PWM          - Cabinets LEDs
  D9/RX
  D8/TX
 
  Right Side
  LI+ 
  EN 
  VSUB (5v)
  D10
  D7
  D6
  D5
  D4
  D3
  D2
  D1/A4/ADC1/PWM/Wire SCL   - NCD I2C
  D0/A3/ADC2/Wire SDA       - NCD I2C
 
  SWITCH WIRING
  Top left:      tape label "Cabinet" -> D3 green -> gold
  Top right:     no label (right trim) #4 yellow
  2nd left:      tape label "Sink" #2 white
  2nd middle:    tape label "Ceiling" (kitchen) #1 red
  2nd right:     tape label "Indirect" (left trim) #5 blue
  gnd            brown?

  3rd left:      "Ceiling" D2 blue -> silver
  3rd l-m:       "DS Flood Lights" #0 green
  3rd r-m:       "ODS Flood Lights" #3 yellow
  3rd right:     "Porch Lights" #7 red
  bottom:        "Light" (awning) #6 white

  TERMINAL STRIPS
   Top: LED Drivers
    *
    *
    *
    *
    *
    *
   Top 12v: Fuse #5 Purple/White
   Gnd
   Middle 12v: Fuse #2 Orange/White
            also Fuse ? Purple/White marked Ceiling Power
   Bottom 12v: Fuse #14 Gray/White Awning
 
 Fuse Panel
 1. Brown/White     Bedroom Lts.
 2. Orange/White    Puck Lts (Ceiling Power)
 3. Green/White     Storage Lts
 4. Blue/White      Br Slide out
 5. Purple/White    Kitchen (Front Panel)
 6. Pink solid      Furnace
 7. Yellow/White    Fan
 8. Pink/White      Fuel Pump
 9. Blue solid      Water Pump
 10. Black/White    Monitor Panel
 11. Green/White    Ent
 12. n/c
 13. Red/White      Garage (RearPanel)
 14. Gray/White     Awning
 15. New Red        Pepwave
 16. n/c
 17. n/c
 18. n/c
 
  Photon 2 is 0a10aced202194944a0446ac front_panel2
  RSSI = -43dBm on 2/19/24
 
  Original Photon was 430033000f47343339383037 FrontPanel
  RSSI = -66dBm  on 10/1/22
 
 History
 02/19/24 Upgrade to new architecture
 10/13/23 Upgrade NCD 8x dimmer with 16x board (12-bit)
 
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
