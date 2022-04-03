/**
This is the primary class for the Patriot IoT library.

It aggregates all the other classes, and provides
a common API for adding and configuring devices.

This class coordinates realtime events.
It subscribes to Particle.io notifications, and
        distributes them to devices.

http://www.github.com/rlisle/Patriot

Written by Ron Lisle

BSD license, check LICENSE for more information.
All text above must be included in any redistribution.

*/
#include "IoT.h"

// Static Variables
Device*      Device::_devices = NULL;
MQTTManager* IoT::_mqttManager = NULL;

/**
 * Begin gets everything going.
 * It must be called exactly once by the sketch
 */
void IoT::begin(String brokerIP, String controllerName)
{
    String connectID = controllerName + "Id";
    _mqttManager = new MQTTManager(brokerIP, connectID, controllerName);
    
    // Subscribe to events. There is a 1/second limit for events.
    Particle.subscribe(kPublishName, IoT::subscribeHandler, MY_DEVICES);

    // Expose particle.io variables
    Device::expose();
    
}

void IoT::mqttPublish(String topic, String message)
{
    if (_mqttManager != NULL) {
        _mqttManager->publish(topic, message);
    }
}

/**
 * Loop method must be called periodically,
 * typically from the sketch loop() method.
 */
void IoT::loop()
{
    Device::loopAll();

    if (_mqttManager != NULL) {
        _mqttManager->loop();
    }

    int day = Time.day();
    if (Time.day() != _prevDay) {
        // Handle any once-a-day things
        // TODO:
        //if (Time.month() == 3 && )
        
    }
}

/**
 * Particle.io Subscribe Handler
 * t:patriot m:<device>:<value>
 * This method handles commands from Alexa
 */
void IoT::subscribeHandler(const char *eventName, const char *rawData)
{
    String data = String(rawData).trim();
    String event(eventName);
    
    Log.info("Particle.io subscribe received data: '"+event+"', '"+data+"'");
    
    if (_mqttManager != NULL) {
        _mqttManager->parseMessage(event.toLowerCase(), data.toLowerCase());
    }
}

/**
 MQTT Subscribe Handler
*/
void IoT::mqttHandler(char* rawTopic, byte* payload, unsigned int length) {

    if(_mqttManager != NULL) {
        _mqttManager->mqttHandler(rawTopic, payload, length);
    }
}

/**
 publishValue()
 param: name of state
 param: value to assign state
 return: 0 success, -1 MQTT error
 */
int IoT::publishValue(String name, int value) {
    if(_mqttManager != NULL) {
        _mqttManager->publish("patriot/" + name, String(value));
        return 0;
    }
    return -1;
}
