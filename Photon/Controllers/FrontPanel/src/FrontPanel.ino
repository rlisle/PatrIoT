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
#include "Settings.h"
#include "Behaviors.h"
#include "EventHandlers.h"

void loop() {
    IoT::loop();
}

void setup() {
    setupWifi();    // in Settings.h
    
    // This also sets timezone and DST
    IoT::begin(MQTT_BROKER, CONTROLLER_NAME, MQTT_LOGGING);

    // Required by NCD8Light
    PCA9685::initialize(PCA9685_ADDRESS);

    // Behaviors
    setNextMinuteHandler(handleNextMinute);
    Device::setAnyChangedHandler(updateLights);

    // Be careful that names don't collide with Homebridge (commands)
    Device::add(new Device("AnyoneHome", "Status", 'S'));
    Device::add(new Device("Bedroom", "Status", 'L'));
    Device::add(new Device("Cabinets", "Status", 'S'));
    Device::add(new Device("Cleaning", "Status", 'S'));
    Device::add(new Device("Kitchen", "Status", 'L'));
    Device::add(new Device("LivingRoomMotion", "Status", 'S', handleLivingRoomMotion));
    Device::add(new Device("Nighttime", "Status", 'S'));
    Device::add(new Device("Nook", "Status", 'L'));
    Device::add(new Device("OfficeMotion", "Status", 'S'));
    Device::add(new Device("Outside", "Status", 'S'));
    Device::add(new Device("Retiring", "Status", 'S', handleRetiring));
    Device::add(new Device("RonHome", "Status", 'S'));
    Device::add(new Device("ShelleyHome", "Status", 'S'));
    Device::add(new Device("Sink", "Status", 'L'));
    Device::add(new Device("Sleeping", "Status", 'S', handleSleeping));
    Device::add(new Device("Theatre", "Status", 'S'));

    // Inside Lights    TODO: set actual light #s     TODO: set wire colors
    Device::add(new NCD16Light(15, "KitchenCeiling", "Kitchen",0));   // R
    Device::add(new NCD16Light(13, "SinkLamp", "Kitchen",0));         // B
    Device::add(new NCD16Light(12, "LeftTrim", "Kitchen",0));         // G?
    Device::add(new NCD16Light(16, "RightTrim", "Living Room",0));    // Y?
    Device::add(new NCD16Light(3,  "Ceiling", "Living Room",0));      // W
    Device::add(new NCD16Light(14, "CabinetLamps", "Kitchen",0));     // R

    // Zigbee Lights
    Device::add(new ZigbeeLight("NookLamp", "Living Room"));
    Device::add(new ZigbeeLight("BedroomLamp", "Bedroom"));

    // Outside Lights
    Device::add(new NCD16Light(1, "DoorSide", "Outside",0));          // B
    Device::add(new NCD16Light(5, "OtherSide", "Outside",0));         // G
    Device::add(new NCD16Light(2, "FrontAwning", "Outside",0));       // Y
    Device::add(new NCD16Light(4, "FrontPorch", "Outside",0));        // W

    // Zigbee Contact Switches
    Device::add(new ZigbeeMotion("CoffeeMotion", "Office", handleCoffeeMotion));    //TODO: move to frontpanel

    // 12V Monitor (actually 14.27) with 10:1 R-Ladder
    // Adjust fullScale to reflect actual R-Ladder (36.9)
//    Device::add(new Voltage(A0, "FP volts", "LivingRoom", 36.9, 10));
    
//    Device::add(new Power("Power", "Status"));

    // Inputs - test/debug at this point
    
//    MCP23008::initialize(SWITCH_ADDRESS, SWITCH_IOMAP);

//    Device::add(new NCD8Switch(1, "FrontDoor", "Living Room", handleFrontDoor));
//    Device::add(new NCD8Switch(2, "Input2", "Living Room"));
//    Device::add(new NCD8Switch(3, "Input3", "Living Room"));
//    Device::add(new NCD8Switch(4, "Input4", "Living Room"));
//    Device::add(new NCD8Switch(5, "Input5", "Living Room"));
//    Device::add(new NCD8Switch(6, "Input6", "Living Room"));
//    Device::add(new NCD8Switch(7, "Input7", "Living Room"));
//    Device::add(new NCD8Switch(8, "Input8", "Living Room"));

    //TODO: Initialize state: Nighttime, Sleeping, Doors, etc.


}


//void handleVoltageChanged(int volts) {
//    // Do whatever is needed based on new volts value
//    if(volts >= 12) {
//        Log.info("Voltage is over 12: " + String(volts));
//    } else {
//        Log.info("Voltage is under 12: " + String(volts));
//    }
//}
