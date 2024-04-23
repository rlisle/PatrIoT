/* 
 * Project Patriot RearPanel
 * Author: Ron Lisle
 * Date: 1/5/24
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 * 
 * To update code:
 *  1. Edit this code (VSCode or Xcode)
 *  2. Update IoT if needed
 * If USB connected using VSCode:
 *  3. Put Photon into safe mode using buttons (breathing magenta):
 *     Press both buttons, release reset, release setup when blinking magenta
 *  4. "frp2" or "particle flash rear_panel2"
 * If OTA
 *  3. "frp"
 * 
 * Compiling: particle compile photon2 --target 5.6.0
 * Flashing: particle flash rear_panel2 --target 5.6.0 or shortcut "frp"
 * 
 * 
 * I2CIO4R4G5LE board connected via I2C
 *   Relays: 0, 1 Curtain, 2, 3 unused
 *   GPIO connector: GP4, Gnd, GP5, GP6, Gnd, GP7
 *   GP4 = Door, GP5 = PIR Power, GP6 = PIR Input, GP7 n/c
 * 
 * I2CPWM8W80C  
 */

#include <IoT.h>

// SETTINGS

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

// Try using HomeKit to perform timed light turn-off
// Timing
// bool isTimingOfficeMotion;
// bool isTimingOfficeDoor = false;
// bool isTimingRampDoor = false;

// LOOP
void loop() {
    IoT::loop();

    // if(isTimingOfficeDoor) {
    //     if(millis() > Device::msecsLastChange("OfficeDoor") + officeDoorTimeoutMsecs) {
    //         Log.info("RP turning off isTimingOfficeDoor");
    //         isTimingOfficeDoor = false;
    //         updateLights();
    //     }
    // }

    // if(isTimingOfficeMotion) {
    //     if(Device::msecsLastChange("OfficeMotion") + officeMotionTimeoutMsecs < millis()) {
    //         isTimingOfficeMotion = false;
    //         updateLights();
    //     }
    // }

    // if(isTimingRampDoor) {
    //     if(millis() > Device::msecsLastChange("RampDoor") + rampDoorTimeoutMsecs) {
    //         Log.info("RP turning off isTimingOfficeDoor");
    //         isTimingRampDoor = false;
    //         updateLights();
    //     }
    // }

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

    // Behaviors
    //setNextMinuteHandler(handleNextMinute);
    //Device::setAnyChangedHandler(updateLights);

    //TODO: refactor to array of structs/enums
    // Device::add(new Device("AnyoneHome", "Status", 'S'));
    // Device::add(new Device("Cleaning", "Status", 'S'));
    // Device::add(new Device("Desk", "Status", 'L'));         // Desk override
    // Device::add(new Device("Loft", "Status", 'L'));         // Loft override
    // Device::add(new Device("Nighttime", "Status", 'S', handleNighttime));
    // Device::add(new Device("Office", "Status", 'L'));       // Office override
    // Device::add(new Device("Outside", "Status", 'L'));      // Outside override
    // Device::add(new Device("Patio", "Status", 'L'));        // Porch and awning lights override
    // Device::add(new Device("Piano", "Status", 'L'));        // PianoSpot override
    // Device::add(new Device("Ramp", "Status", 'L'));         // Ramp override
    // Device::add(new Device("Retiring", "Status", 'S', handleRetiring));
    // Device::add(new Device("RonHome", "Status", 'S'));
    // Device::add(new Device("ShelleyHome", "Status", 'S'));
    // Device::add(new Device("Sleeping", "Status", 'S', handleSleeping));
    // Device::add(new Device("Theatre", "Status", 'S'));

    // I2CIO4R4G5LE board
    // 4 Relays
//    Device::add(new Curtain(0, "Curtain", "Office"));     // 2x Relays: 0, 1
    // Device::add(new Awning(2, "RearAwning", "Outside")); // 2x Relays: 2, 3
    
    // 4 GPIO
    Device::add(new NCD4Switch(1, "OfficeDoor", "Office" /*, handleOfficeDoor*/));
    Device::add(new NCD4PIR(3, "OfficeMotion", "Office", officeMotionTimeoutMsecs /*, handleOfficeMotion*/));

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
    Device::add(new ZigbeeContact("RampDoor", "Office" /*, handleRampDoor*/));

    // Zigbee Outlets
    Device::add(new ZigbeeOutlet("OfficeValence", "Office"));

    // setInitialState();
    // updateLights();
}
