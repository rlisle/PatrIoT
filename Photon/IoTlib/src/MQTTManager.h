/**
 MQTTManager.h
 
 This class handles all MQTT interactions.
 
 Note: to avoid making this a singleton,
 the caller must provide global callback handlers (see externs).
 
 http://www.github.com/rlisle/Patriot
 
 Written by Ron Lisle
 
 BSD license, check LICENSE for more information.
 All text above must be included in any redistribution.
 
 */
#pragma once
#include "Particle.h"
#include "MQTT.h"
#include "behaviors.h"
#include "devices.h"
#include "states.h"

class Devices;

class MQTTManager
{
public:
    
    MQTTManager(String brokerIP, String connectID, String controllerName, Devices* devices);
    
    bool        publish(String topic, String message);
    void        parseMessage(String topic, String message);
    void        loop();
    void        mqttHandler(char* topic, byte* payload, unsigned int length);
        
private:
    MQTT      *_mqtt;
    String    _controllerName;
    system_tick_t _lastMQTTtime;
    
    Devices   *_devices;
    
    void      (*_callback)(char*,uint8_t*,unsigned int);
    void      connect(String connectID);
    void      reconnectCheck();
    int       parseValue(String message);
    void      parseLogLevel(String message);
    void      log(String message, PLogLevel logLevel);  // Convenience method - calls IoT->Log()
};
