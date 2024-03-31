/**
 Zigbee motion sensor control
 for use with Sonoff USB Zigbee adapter

 Features:
 - PIR Motion detection

 Leverage capability of Sonoff SNZB-03 Wireless Motion Sensor.
 
 MQTT:
   patriot/zigbee/<name> {..., "occupancy": true/false, ...}
   eg. patriot/zigbee/CoffeeMotion {"battery":100,"battery_low":false,"linkquality":255,"occupancy":true,"tamper":false,"voltage":3400}

 http://www.github.com/rlisle/Patriot

 Written by Ron Lisle

 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 Datasheets:
 - https://www.zigbee2mqtt.io/devices/SNZB-03.html

 */

#include "IoT.h"

/**
 * Constructor
 * @param name String name used to address the motion sensor.
 */
ZigbeeMotion::ZigbeeMotion(String name, String room, void (*handler)(int,int))
        : Device(name, room, 'M', handler)
{
    // Nothing to do
}

void ZigbeeMotion::begin() {
    // Nothing to do
}

// Only subtopic following patriot/zigbee/ is passed.
// This will be the lower cased device name
void ZigbeeMotion::mqtt(String topic, String message) {
//    Log.info("ZigbeeMotion %s mqtt received: %s %s",name().c_str(),topic.c_str(),message.c_str());
    if(topic.equalsIgnoreCase(name())) {  
        Log.info("Zigbee motion sensor message to us %s",topic.c_str());

        // Now parse message for specific fields
        int occupancyIndex = message.indexOf("occupancy");
        if(occupancyIndex > 0) {
            String occupancy = message.substring(occupancyIndex+11,occupancyIndex+15); // "true" or "fals"
            Log.info("occupancy = %s", occupancy.c_str());
            _value = (occupancy == "true") ? 100 : 0;
        }
    }
}

/**
 * loop()
 * Called periodically to perform dimming, polling, etc.
 * but this is done by the Sengled light bulb itself.
 */
void ZigbeeMotion::loop()
{
    //Nothing to do
};
