/******************************************************************
 HueLight plugin

 Features:
 - Control Philips Hue light.

http://www.github.com/rlisle/Patriot
 
Example code used from https://www.digikey.com/en/maker/blogs/2019/how-to-post-data-using-the-particle-photon

Written by Ron Lisle

BSD license, check license.txt for more information.
All text above must be included in any redistribution.

******************************************************************/

#include "HueLight.h"
#include "IoT.h"

/**
 * Constructor
 * @param name  String name of the checklist item
 */
HueLight::HueLight(String name, String room, String hueId, byte *server, String userid)
        : Device(name, room)
{
    _hueId = hueId;
    memcpy(_server, server, 4);
    _userID = userid;
    _value = 0;
    _type  = 'L';
}

void HueLight::begin() {
    if(_tcpClient.connect(_server,80)) {
        Log("HueLight "+_name+" connected");
        _tcpClient.stop();
    } else {
        Log("HueLight "+_name+" failed to connect!");
    }
    
}

/**
 * loop()
 * This is called repeatedly to allow device to do its work.
 */
void HueLight::loop()
{
    //Nothing to do?
}

/**
 * Set value
 * @param value Int 0 to 100
 */
void HueLight::setValue(int value) {
    Log.info("HueLight " + _name + " setValue: " + String(value));
    if(_value == value) {
        Log.info("HueLight value already set, but writing again!");
    }
    _value = value;
    writeToHue();
}


// Private Helper Methods
// This is currently just simple on/off.
// Much more is possible through the API.
void HueLight::writeToHue() {
    //TODO: lookup ID from name
    if(_tcpClient.connect(_server,80)) {

        String json = _value > 0 ? "{\"on\":true}" : "{\"on\":false}";

        _tcpClient.print("PUT /api/");
        _tcpClient.print(_userID);
        _tcpClient.print("/lights/");
        _tcpClient.print(_hueId);
        _tcpClient.println("/state HTTP/1.0");
        
        _tcpClient.print("Content-Length: ");
        _tcpClient.println(json.length());
        _tcpClient.println();
        _tcpClient.println(json);
    } else {
        Log("HueLight "+_name+" not connected");
    }
}
/**
 * notify()
 * Publish switch state
 */
void HueLight::notify()
{
    String topic = "patriot/" + _name;
    String message = String(_value);
    IoT::mqttPublish(topic,message);
}