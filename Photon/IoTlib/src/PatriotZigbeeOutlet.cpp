/**
 Zigbee Outlet control
 for use with Sonoff USB Zigbee adapter

 Features:
 - 120vac outlet control

 MQTT
 patriot/zigbee/<device>/set {"state": "ON/OFF"}
 eg. ppub zigbee/OfficeValence/set {/"state/":/"ON/"}

 Leverage capability of Sonoff S40 Lite Zigbee Smart Plug
 
 http://www.github.com/rlisle/Patriot

 Written by Ron Lisle

 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 Datasheets:
 https://www.zigbee2mqtt.io/devices/S40ZBTPB.html

 */

#include "IoT.h"

/**
 * Constructor
 * @param name String name used to address the outlet.
 */
ZigbeeOutlet::ZigbeeOutlet(String name, String room)
        : Device(name, room)
{
    _type               = 'S';  // switch TODO: O = outlet
}

void ZigbeeOutlet::begin() {
    // Nothing to do
}

/**
 * Set value
 * @param value Int 0 to 100
 */
void ZigbeeOutlet::setValue(int value) 
{
    //Convert percent to ON/OFF
    String outputValue = value > 0 ? "ON" : "OFF";
    // output MQTT - IoT::publishMQTT(subtopic, message, retain=false)
    // patriot/zigbee/<name>/set -m {"state":"ON"} - ON/OFF/TOGGLE
    // optionally include on_time 
    String topic = "zigbee/" + _name + "/set";
    String state = "{\"state\":\"" + outputValue + "\"}";
    IoT::publishMQTT(topic, state);
    _value = value;
}

/**
 * loop()
 * Called periodically to perform dimming, polling, etc.
 * but this is done by the Sengled light bulb itself.
 */
void ZigbeeOutlet::loop()
{
    //Nothing to do
};
