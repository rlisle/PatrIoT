/******************************************************************
Device class

This class is the parent of all device classes
such as Lights, Switches, Fans, etc.

http://www.github.com/rlisle/Patriot
 
 Device types:
 * C curtain
 * D door switch
 * F fan
 * H humidity
 * L light
 * M motion detector
 * P presence/proximity detector
 * S switch
 * T temperature
 * V voltage
 * W power
 * X checklist state

Written by Ron Lisle

BSD license, check license.txt for more information.
All text above must be included in any redistribution.

******************************************************************/
#pragma once

#include "Particle.h"

typedef unsigned long msecs;

class Device {
 protected:
    Device*    _next;       // Linked list
    String     _name;       // Public name (used by Alexa)
    String     _room;       // Room where located
    int        _value;      // Current value, typically percent 0-100
    int        _previous;   // Value before getChanged called
    msecs      _msecsLastChange; // MSecs since last value change
    char       _type;       // Character designating device type: L, S, etc.
    int        _brightness; // Persisted value to use with next setOn (used by IoT, not device itself)
    void       (*_changeHandler)(int, int);  // Called whenever value changes, passing new and old values

public:

    Device(String name, String room, char type = 'L', void (*handler)(int,int) = NULL);

    // begin() is called automatically when device is added.
    // Do any heavy lifting or publishing here and not in constructor.
    virtual void begin() {};
    
    // Perform hardware reset if possible
    virtual void reset() {};

    virtual String name() { return _name; };
    virtual String room() { return _room; };
    virtual String type() { return String(_type); };
    
    virtual int value() { return _value; };
    virtual void setValue(int value);
    
    virtual void setHold(bool stopped) { return; };     // eg. Pause closing curtain
    
    virtual int brightness() { return _brightness; };
    virtual void setBrightness(int value);

//    virtual int  getChangedValue();
    
    virtual void setLatLong(float latitude, float longitude);

    // Perform things continuously, such as fading or slewing
    virtual void loop() {};

    // Each plugin can receive and parse MQTT if needed
    virtual void mqtt(String topic, String message);    
    
    //
    // Static Collection Methods (previously in Devices)
    //
    static Device* _devices;

    static void (*_anyChangeHandler)();  // Called whenever value changes, passing new and old values
    
    static void resetAll();
    static void loopAll();

    static void    add(Device *device);
    static Device* get(String name);
    static int     value(String name);
    static int     setValue(String name, int value);
//    static int     getChangedValue(String name);
    static int     count();
    static void    setAllLatLong(float latitude, float longitude);

    static msecs   msecsLastChange(String name);

    static void    mqttAll(String topic, String message);

    // Called once whenever ANY device value has changed
    static void    setAnyChangedHandler(void (*handler)());

    /**
     Particle.io variable "States"
     */
    static void publishStates();
    static void expose();
    static int setSSID(String info);
    static int setTimeZone(String info);
    static int updateAllLights(String info);

    static String calculateChecklist();
    static String calculateStatus();
    static String calculateOnStatus();
    static String calculateOffStatus();
    static String calculateDevices();
};
