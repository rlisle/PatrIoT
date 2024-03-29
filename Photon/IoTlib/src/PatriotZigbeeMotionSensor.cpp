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

void ZigbeeMotion::mqtt(String topic, String message) {
    // Parse patriot/zigbee/<device> 
    // {"battery":100,"battery_low":false,"linkquality":255,"occupancy":true,"tamper":false,"voltage":3400}
    String subtopics[5];
    int start = 0;
    int end = topic.indexOf('/');
    int numTopics = 0;
    if(end > 0) { // Might be -1 if only 1 subtopic
        do {
            start = end+1;
            end = topic.indexOf('/', start);
            numTopics++;
        } while(numTopics < 4 && end > 0);
    }
    subtopics[numTopics++] = topic.substring(start);  // Last one
    
    if(numTopics == 2 && subtopics[0] == "zigbee") {
        Log.info("DEBUG: zigbee message received");
        if(subtopics[1] == name()) {  
            Log.info("Zigbee motion sensor message to us");
            //TODO: locate "occupancy" 

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
