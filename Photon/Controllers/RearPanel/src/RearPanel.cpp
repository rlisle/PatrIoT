/* 
 * Project Patriot RearPanel
 * Author: Ron Lisle
 * Date: 1/5/24
 */

#include <IoT.h>

// Generally uncomment only 1 of the following 2 logs
#define MQTT_LOGGING true
// View logs with CLI using 'particle serial monitor --follow'
//SerialLogHandler logHandler1(57600, LOG_LEVEL_INFO);

//TODO: change to const
#define CONTROLLER_NAME "RearPanel"
#define MQTT_BROKER "192.168.0.33"

int const officeMotionTimeoutMsecs = 60*1000;
int const officeDoorTimeoutMsecs = 7*60*1000;
int const rampDoorTimeoutMsecs = 7*60*1000;

#define PCA9634_ADDRESS 1       // 1st jumper
#define I2CR4IO4_ADDRESS 0x20  // 4xRelay+4GPIO address (0x20 = no jumpers)

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(AUTOMATIC);

void loop() {
    IoT::loop();
}

// SETUP
void setup() {

//    WiFi.setCredentials(WIFI_SSID, WIFI_PASSWORD);
//    WiFi.selectAntenna(ANT_INTERNAL);    
//    WiFi.useDynamicIP();

    IoT::begin(MQTT_BROKER, CONTROLLER_NAME, MQTT_LOGGING);
    
    //Consolidate PCA9634 initialization
    // GPIO4 = Door, GPIO5 = PIR Power, GPIO6 = PIR Input, GPIO7 n/c
    MCP23008::initialize(I2CR4IO4_ADDRESS, 0xd0);   // Address 0x20 (no jumpers)
    PCA9634::initialize(PCA9634_ADDRESS);

    MCP23008::write(5,true);   // Apply power to PIR. Pin can source 25ma

    // I2CIO4R4G5LE board
    // 4 Relays
//    Device::add(new Curtain(0, "Curtain", "Office"));     // 2x Relays: 0, 1
    
    // 4 GPIO
    Device::add(new NCD4Switch(1, "OfficeDoor", "Office"));
    Device::add(new NCD4PIR(3, "OfficeMotion", "Office", officeMotionTimeoutMsecs));

    // I2CPWM8W80C board
    // 8 Dimmers
    Device::add(new NCD8Light(1, "OfficeCeiling", "Office"));
    Device::add(new NCD8Light(2, "Loft", "Office"));
    Device::add(new NCD8Light(3, "RampPorch", "Outside"));
    Device::add(new NCD8Light(4, "RampAwning", "Outside"));
    Device::add(new NCD8Light(5, "RearPorch", "Outside"));
    Device::add(new NCD8Light(6, "RearAwning", "Outside"));
    Device::add(new NCD8Light(7, "Piano", "Office"));
    //Device::add(new NCD8Light(8, "Unused", "Office"));

    // Zigbee Lamps
    Device::add(new ZigbeeLight("LeftDeskLamp", "Office"));
    Device::add(new ZigbeeLight("RightDeskLamp", "Office"));

    // Zigbee Contact Switches
    Device::add(new ZigbeeContact("RampDoor", "Office"));

    // Zigbee Outlets
    Device::add(new ZigbeeOutlet("OfficeValence", "Office"));
}
