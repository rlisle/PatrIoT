/**
 PatriotNCD4Switch

 Features:
 - Read and debounce switch input

 http://www.github.com/rlisle/Patriot

 Written by Ron Lisle

 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 Datasheets:
   MCP23008 https://cdn-shop.adafruit.com/datasheets/MCP23008.pdf
 
 Changelog:
 2023-09-24: Use separate MCP23008 plugin
 2021-01-16: Initial creation
 2022-12-10: Change MQTT message to patriot/<device>/get/position
 2023-08-10: Refactor MCP23008 to separate class
 */

#include "IoT.h"

#define POLL_INTERVAL_MILLIS 100

/**
 * Constructor
 * @param switchIndex is the switch number on the NCD board (0-3)
 * @param name String name used to address the relay.
 */
//Device(String name, String room, char type = 'L', void (*handler)(int,int) = NULL);
NCD4Switch::NCD4Switch(int8_t switchIndex, String name, String room, void (*handler)(int,int))
    : Device(name, room, 'S', handler)
{
    _lastPollTime = 0;
    _type         = 'S';
    
    if(switchIndex > 0 && switchIndex <= 4) {   // Index is 1-4 (1 based)
        _switchBitmap = 0x10 << (switchIndex-1);
    } else {
        _switchBitmap = 0x10;   // If 0 or invalid, set to first switch
    }
}

/**
 * isSwitchOn
 * Return state of switch (inverted: low = 100, high = 0)
 */
bool NCD4Switch::isSwitchOn() {
    int data = MCP23008::read();
    return(data & _switchBitmap);    // Inverted by IPOL register
}

/**
 * loop()
 */
void NCD4Switch::loop()
{
    if (isTimeToCheckSwitch())
    {
        if (didSwitchChange())
        {
            notify();
        }
    }
}

// Private Helper Methods
/**
 * isTimeToCheckSwitch()
 * @return bool if enough time has elapsed to sample switch again
 */
bool NCD4Switch::isTimeToCheckSwitch()
{
    long currentTime = millis();
    if (currentTime < _lastPollTime + POLL_INTERVAL_MILLIS)
    {
        return false;
    }
    _lastPollTime = currentTime;
    return true;
}


/**
 * didSwitchChange()
 * @return bool if switch has changed since last reading
 */
bool NCD4Switch::didSwitchChange()
{
    int newValue = isSwitchOn() ? 100 : 0;
    bool oldState = (_value != 0);
    
    if(oldState == false && newValue == 100) {
        Log.info("Switch turned on");
        _value = 100;
        return true;
    }
    
    if(oldState == true && newValue == 0) {
        Log.info("Switch turned off");
        _value = 0;
        return true;
    }
    
    return false;
}


/**
 * notify()
 * Publish switch state
 */
void NCD4Switch::notify()
{
    String message = String(_value);
    IoT::publishMQTT(_name + "/get/position",message);
}

