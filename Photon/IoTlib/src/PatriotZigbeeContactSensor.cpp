/**
 Zigbee contact sensor control
 for use with Sonoff USB Zigbee adapter

 Features:
 - Magnetic contact detection

 Leverage capability of Sonoff SNZB-04 Wireless Door/Window Sensor.
 
 http://www.github.com/rlisle/Patriot

 Written by Ron Lisle

 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 Datasheets:

 */

#include "IoT.h"

/**
 * Constructor
 * @param name String name used to address the light.
 */
ZigbeeContact::ZigbeeContact(String name, String room, void (*handler)(int,int))
        : Device(name, room, 'D', handler)
{
    // Nothing to do
}

void ZigbeeContact::begin() {
    // Nothing to do
}

// Only subtopic following patriot/zigbee/ is passed.
// This should be the lower cased device name
void ZigbeeContact::mqtt(String topic, String message) {
    // Parse patriot/zigbee/<device> 
    // {"battery":100,"battery_low":false,"contact":true/false,"linkquality":156,"tamper":false,"voltage":3200}
    if(topic.equalsIgnoreCase(name())) {  
        Log.info("Zigbee contact sensor message to us: %s",topic.c_str());

        // Now parse message for specific fields
        int contactIndex = message.indexOf("contact");
        if(contactIndex > 0) {
            String contact = message.substring(contactIndex+9,contactIndex+13); // "true" or "fals"
            Log.info("contact = %s", contact.c_str());
            _value = (contact == "true") ? 100 : 0;
        }
    }
}

/**
 * loop()
 * Called periodically to perform dimming, polling, etc.
 * but this is done by the Sengled light bulb itself.
 */
void ZigbeeContact::loop()
{
    //Nothing to do
};
