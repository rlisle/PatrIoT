/**
Device parent class

This is the parent class of all device classes.
It provides static methods for managing the collection of devices.

http://www.github.com/rlisle/Patriot

Written by Ron Lisle
BSD license, check license.txt for more information.
All text above must be included in any redistribution.

*/
#include "IoT.h"
#include "constants.h"

void (*Device::_anyChangeHandler)() = NULL;

Device::Device(String name, String room, char type, void (*handler)(int,int))
: _next(NULL), _name(name), _room(room), _value(0), _previous(0), _type(type), _brightness(100), _changeHandler(handler)
{
    // Do any setup work in begin() not here.
}

// Actual device will override this in most cases
void Device::setValue(int value) {
    _value = value;
}

void Device::setBrightness(int value) {
    _brightness = value;
    // Setting brightness should not turn of a device that is currently off
    // but it will adjust the level of one that is already on
    if(_value != 0) {
        setValue(value);
    }
}

// Check if device has changed and return new value or -1
// int  Device::getChangedValue() {
//     if( _value == _previous ) {
//         //Don't put a log here. This is called almost every loop.
//         return -1;
//     }
//     // Log here is ok because it only occurs when value changes
//     //Log.info("Device "+_name+" getChangedValue() changed");
//     _previous = _value;
//     return _value;
// }

// Static Methods

void Device::setAnyChangedHandler(void (*handler)()) {
    _anyChangeHandler = handler;
}

void Device::add(Device *device)
{
    Log.info("addDevice name: "+String(device->name()));
    if(_devices == NULL) {
        Log.info("  first device");
        _devices = device;
    } else {
        Device *ptr = _devices;
        int i = 1;
        while(ptr->_next != NULL) {
            ptr = ptr->_next;
            i++;
        }
        ptr->_next = device;
    }
    device->begin();
}

void Device::resetAll()
{
    for (Device *ptr = _devices; ptr != NULL; ptr = ptr->_next)
    {
        // TODO: call reset only once per board, not device
        ptr->reset();
    }
}

void Device::loopAll()
{
    bool didAnyChange = false;
    for (Device *ptr = _devices; ptr != NULL; ptr = ptr->_next)
    {
        ptr->loop();
        if(ptr->_value != ptr->_previous) {
            Log.info("Device::loopAll device %s changed %d != %d", ptr->_name.c_str(), ptr->_value, ptr->_previous);
            didAnyChange = true;
            ptr->_msecsLastChange = millis();
            if(ptr->_changeHandler != NULL) {
                ptr->_changeHandler(ptr->_value, ptr->_previous);
            }
            ptr->_previous = ptr->_value;
            Log.info("Device::loopAll device %s changed previous = value %d", ptr->_name.c_str(), ptr->_previous);
        }
    }
    if(didAnyChange == true && _anyChangeHandler != NULL) {
        Log.info("Device::loopAll calling anyChangeHandler");
        Device::_anyChangeHandler();
    }
}

Device *Device::get(String name)
{
    Device *ptr = _devices;
    for (int i = 0; i < count() && ptr != NULL; i++)
    {
        if (ptr->name().equalsIgnoreCase(name)) {
            return ptr;
        }
        ptr = ptr->_next;
    }
    return NULL;
}

int Device::value(String name) {
    Device *ptr = get(name);
    if( ptr == NULL ) return -1;
    return( ptr->value() );
}

int Device::setValue(String name, int value) {
    Device *ptr = get(name);
    if( ptr == NULL ) return -1;
    ptr->setValue(value);
    return 0;
}

// Check if device has changed and return new value or -1
// int  Device::getChangedValue(String name) {
//     Device *device = get(name);
//     if( device == NULL ) {
//         return -1;
//     }
//     return device->getChangedValue();
// }

msecs Device::msecsLastChange(String name) {
    Device *device = get(name);
    if( device == NULL ) {
        return 0;
    }
    return device->_msecsLastChange;
}

void Device::setLatLong(float latitude, float longitude) {
    // Nothing to do. Device will override if needed.
}

void Device::mqtt(String topic, String message) {
    // Nothing to do. Device will override if needed.
}

void Device::setAllLatLong(float latitude, float longitude) {
    Device *ptr = _devices;
    for (int i = 0; i < count() && ptr != NULL; i++)
    {
        ptr->setLatLong(latitude,longitude);
        ptr = ptr->_next;
    }
}

int Device::count()
{
    int i = 0;
    for (Device *ptr = _devices; ptr != NULL; ptr = ptr->_next) i++;
    return i;
}

// Receives MQTT messages that start with "patriot/zigbee/"
void Device::mqttAll(String topic, String message)
{
    for (Device *ptr = _devices; ptr != NULL; ptr = ptr->_next)
    {
        ptr->mqtt(topic, message);
    }
}


// Particle.io Devices, Checklist, and Status variables

String Device::calculateOnStatus() {
    String status = "";
    for (Device* ptr = _devices; ptr != NULL; ptr = ptr->_next) {
        if(ptr->_type != 'X' && ptr->_value > 0) {     // Ignore checklist items
            if(status != "") status += ", ";
            status += String(ptr->_name);
        }
    }
    if(status.length() >= particle::protocol:: MAX_VARIABLE_VALUE_LENGTH) {
        return("Status variable is too long. Need to extend to a 2nd variable");
    }
    return status;
}

String Device::calculateOffStatus() {
    String status = "";
    for (Device* ptr = _devices; ptr != NULL; ptr = ptr->_next) {
        if(ptr->_type != 'X' && ptr->_value == 0) {     // Ignore checklist items
            if(status != "") status += ", ";
            status += String(ptr->_name);
        }
    }
    if(status.length() >= particle::protocol:: MAX_VARIABLE_VALUE_LENGTH) {
        return("Status variable is too long. Need to extend to a 2nd variable");
    }
    return status;
}

String Device::calculateStatus() {
    String status = Time.timeStr() + ": ";

    status += "POD:" + String(partOfDay());
    
    for (Device* ptr = _devices; ptr != NULL; ptr = ptr->_next) {

        if(ptr->_type != 'X') {     // Ignore checklist items
            status += ", ";
            status += String(ptr->_type)+":";
            status += String(ptr->_name);
            status += "="+String(ptr->_value);
        }
    }
    if(status.length() >= particle::protocol:: MAX_VARIABLE_VALUE_LENGTH) {
        return("Status variable is too long. Need to extend to a 2nd variable");
    }
    return status;
}

void Device::expose()
{
    if(!Particle.variable(kDevicesVariableName, calculateDevices))
    {
        Log.error("Error: Unable to expose " + kDevicesVariableName + " variable");
    }
    if(!Particle.variable(kChecklistVariableName, calculateChecklist))
    {
        Log.error("Error: Unable to expose " + kChecklistVariableName + " variable");
    }
    if(!Particle.variable(kOnStatusVariableName, Device::calculateOnStatus))
    {
        Log.error("Error: Unable to expose " + kStatusVariableName + " variable");
    }
    if(!Particle.variable(kOffStatusVariableName, Device::calculateOffStatus))
    {
        Log.error("Error: Unable to expose " + kStatusVariableName + " variable");
    }
    if(!Particle.variable(kStatusVariableName, Device::calculateStatus))
    {
        Log.error("Error: Unable to expose " + kStatusVariableName + " variable");
    }
    if(!Particle.function(kSSIDFunctionName, Device::setSSID))
    {
        Log.error("Error: Unable to expose " + kSSIDFunctionName + " function");
    }
    if(!Particle.function(kTimeZoneFunctionName, Device::setTimeZone))
    {
        Log.error("Error: Unable to expose " + kTimeZoneFunctionName + " function");
    }
}

// Set SSID & Password info using CSL
int Device::setSSID(String info) {
    int start = 0;
    int comma = info.indexOf(',');
    if(comma <= 0) {  // -1 if comma missing
        Log.error("setSSID info incorrect: " + info);
        return -1;
    }
    String ssid = info.substring(start, comma);
    String password = info.substring(comma+1);
    Log.info("Setting SSID/password: " + ssid + ", " + password);
    return WiFi.setCredentials(ssid, password);
}

// Set Timezone
int Device::setTimeZone(String info) {
    int tz = info.toInt();
    Log.info("Setting timezone: %d",tz);
    IoT::setTimezone(tz);
    IoT::handleDaylightSavings();
    return 0;
}

// The Devices variable is used by Alexa discovery and ReportState and iOS app.
// It is a comma delimited list of <T>:<Name>
// Note: Alexa skill hasn't been updated to support @<room>, so removing it for now.
String Device::calculateDevices()
{
    String devices = "";
    
    for (Device* ptr = _devices; ptr != NULL; ptr = ptr->_next) {

        if(ptr->_type != 'X') {     // Ignore Checklist items
            devices += String(ptr->_type)+":";
            devices += String(ptr->_name);
            if (ptr->_next != NULL) {
                devices += ",";
            }
        }
    }
    if(devices.length() >= particle::protocol:: MAX_VARIABLE_VALUE_LENGTH) {
        return("Devices variable is too long. Need to extend to a 2nd variable");
    }
    return devices;
}

// The Checklist variable is used by the Checklist iOS app.
// It is a comma delimited list of <Name>=0|1
String Device::calculateChecklist()
{
    String newChecklist = "";
    
    for (Device* ptr = _devices; ptr != NULL; ptr = ptr->_next) {
        if(ptr->_type == 'X') {
            newChecklist += String(ptr->_name);
            newChecklist += "="+String(ptr->_value);
            if (ptr->_next != NULL) {
                newChecklist += ",";
            }
        }
    }
    if(newChecklist.length() >= particle::protocol:: MAX_VARIABLE_VALUE_LENGTH) {
        Log.error("Checklist variable is too long. Need to extend to a 2nd variable");
    }
    if(newChecklist.length() == 0) { Log.info("No checklist items"); }
    return newChecklist;
}


// Publish each device name and its value in response to patriot/query
// New format: patriot/state/<room>/<T>/<name>
void Device::publishStates() {
    for (Device* ptr = _devices; ptr != NULL; ptr = ptr->_next) {
        IoT::publishMQTT("state/"+ptr->room()+"/"+ptr->type()+"/"+ptr->name(), String(ptr->_value));
    }
}
