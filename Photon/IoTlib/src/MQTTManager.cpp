/**
MQTTManager.cpp

This class handles all MQTT interactions.

http://www.github.com/rlisle/Patriot

Written by Ron Lisle

BSD license, check LICENSE for more information.
All text above must be included in any redistribution.

*/
#include "MQTTManager.h"
#include "constants.h"
#include "device.h"
#include "IoT.h"

#define MQTT_TIMEOUT_SECONDS 60*31
#define MQTT_ALIVE_SECONDS 60*15

MQTTManager::MQTTManager(String brokerIP, String connectID, String controllerName)
{
    int month = Time.month();
    int day = Time.day();
    
    _controllerName = controllerName.toLowerCase();
    _logging = 0;

    // We'll want to start with ALL whenever modifying code.
    // Use MQTT to switch to error when done testing or vs. a vs.
//    _logLevel = LOG_LEVEL_ERROR;
    _logLevel = LOG_LEVEL_ALL;

    //TODO: Use GPS to determine actual timezone
    Time.zone(-6.0);    // Set timezone to Central
    
    //TODO: Set Daylight Savings Time as needed
    // Begins on the 2nd Sunday in March at 2:00 am
    // Ends on the 1st Sunday in November
    // 2021: 3/14 - 11/7
    // 2022: 3/13 - 11/6
    // 2023: 3/12 - 11/5
    //TODO: call beginDST() if between those dates
    
    //TODO: do we need this, and what should we pass?
    //const LogCategoryFilters &filters) : LogHandler(level, filters)

    _mqtt =  new MQTT((char *)brokerIP.c_str(), 1883, IoT::mqttHandler);
    connect(connectID);
}

//TODO: If MQTT doesn't connect, then start 
void MQTTManager::connect(String connectID) {

    _connectID = connectID;
    _lastMQTTtime = Time.now();
    _lastAliveTime = _lastMQTTtime;

    if(_mqtt == NULL) {
        Log.error("ERROR! MQTTManager: connect called but object null");
    }

    if(_mqtt->isConnected()) {
        Log.info("MQTT is connected, so reconnecting...");
        LogManager::instance()->removeHandler(this);
        _mqtt->disconnect();
    }

    _mqtt->connect(connectID);
    if (_mqtt->isConnected()) {
        if(_mqtt->subscribe(kPublishName+"/#") == false) {
            Log.error("Unable to subscribe to MQTT " + kPublishName + "/#");
        }
    } else {
        // This won't do anything because our handler isn't connected yet.
        Log.error("MQTT is NOT connected! Check MQTT IP address");
    }
    // Looks good, now register our MQTT LogHandler
    LogManager::instance()->addHandler(this);

    Log.info("MQTT Connected");
    
}

bool MQTTManager::publish(String topic, String message) {
    if(_mqtt != NULL && _mqtt->isConnected()) {
        _mqtt->publish(topic,message);
        return true;
    }
    return false;
}

void MQTTManager::loop()
{
    if(_mqtt != NULL && _mqtt->isConnected()) {
        _mqtt->loop();
        sendAlivePeriodically();
    }

    reconnectCheck();
}

void MQTTManager::sendAlivePeriodically() {
    system_tick_t secondsSinceLastAlive = Time.now() - _lastAliveTime;
    if(secondsSinceLastAlive > MQTT_ALIVE_SECONDS) {
        _lastAliveTime = Time.now();
        String time = Time.format(Time.now(), "%a %H:%M");
        publish("patriot/alive/"+_controllerName, time);
    }
}

void MQTTManager::reconnectCheck() {
    system_tick_t secondsSinceLastMessage = Time.now() - _lastMQTTtime;
    if(secondsSinceLastMessage > MQTT_TIMEOUT_SECONDS) {
        Log.warn("Connection lost, reconnecting. _lastMQTTtime = " + String(_lastMQTTtime) + ", Time.now() = " + String(Time.now()));
        connect(_connectID);
    }
}

void MQTTManager::mqttHandler(char* rawTopic, byte* payload, unsigned int length) {

    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = 0;
    String message(p);
    String topic(rawTopic);

    _lastMQTTtime = Time.now();

    parseMessage(topic.toLowerCase(), message.toLowerCase());
}

//Mark - Parser

void MQTTManager::parseMessage(String lcTopic, String lcMessage)
{
    // This creates an infinite loop. Don't do it.
    //log("Parser received: " + lcTopic + ", " + lcMessage, LogDebug);
    
    // New Protocol: patriot/<name>  <value>
    if(lcTopic.startsWith(kPublishName+"/")) {
        String subtopic = lcTopic.substring(kPublishName.length()+1);
        
        // Look for reserved names
        // ALIVE
        if(subtopic.startsWith("alive")) {
            // Remainder of topic is controller name
            // message is timestamp
            // Ignore it.

        // LOG
        } else if(subtopic == "log" || subtopic.startsWith("log/")) {
            // Ignore it.

        // LOGLEVEL
        } else if(subtopic.startsWith("loglevel")) {
            if(subtopic == "loglevel/"+_controllerName) {
                Log.info(_controllerName + " setting logLevel = " + lcMessage);
                parseLogLevel(lcMessage);
            }
            
        // MEMORY
        } else if(subtopic == "memory") {
            if(lcMessage == _controllerName) {
                publish( "debug/"+_controllerName, String::format("Free memory = %d", System.freeMemory()));
            }
            
        // PING
        } else if(subtopic == "ping") {
            // Respond if ping is addressed to us
            if(lcMessage == _controllerName) {
                Log.trace("Ping addressed to us");
                publish(kPublishName + "/pong", _controllerName);
            }
            
        // PONG
        } else if(subtopic == "pong") {
            // Ignore it.
            
        // QUERY
        } else if(subtopic == "query") {   // was "states"
            if(lcMessage == _controllerName || lcMessage == "all") {
                Log.info("Received query addressed to us");
                Device::publishStates();
            }
                
        // RESET
        } else if(subtopic == "reset") {
            // Respond if reset is addressed to us
            if(lcMessage == _controllerName) {
                Log.info("Reset addressed to us");
                Device::resetAll();
                System.reset(RESET_NO_WAIT);
            }
                
        // STATE
        } else if(subtopic == "state") {
            // Ignore it (for now).
                
        // DEVICE
        } else {
            
            int value = parseValue(lcMessage);
            Device *device = Device::get(subtopic);
            if( device != NULL ) {
                
                // Handle save/restore value
                Log.info("Parser setting device " + subtopic + " to " + value);
                device->setValue(value);
                Device::buildDevicesVariable();
                
//            } else {
//                Log.info("Parsed unknown subtopic "+subtopic);
            }
        }
    } else {
        // Not addressed or recognized by us
        Log.error("Parser: Not our message: "+String(lcTopic)+" "+String(lcMessage));
    }
}

int MQTTManager::parseValue(String lcMessage)
{
    if(lcMessage == "on") {
        return 100;
    } else if(lcMessage == "off") {
        return 0;
    }
    return lcMessage.toInt();
}

void MQTTManager::parseLogLevel(String lcMessage) {
    LogLevel level = LOG_LEVEL_ERROR;
    if (lcMessage == "none") level = LOG_LEVEL_NONE;         // 70
    else if (lcMessage == "error") level = LOG_LEVEL_ERROR;  // 50
    else if (lcMessage == "warn" || lcMessage == "warning") level = LOG_LEVEL_WARN;    // 40
    else if (lcMessage == "info") level = LOG_LEVEL_INFO;    // 30
    else if (lcMessage == "trace") level = LOG_LEVEL_TRACE;  // 1
    else if (lcMessage == "all") level = LOG_LEVEL_ALL;      // 1
    else return;

    _logLevel = level;
}

// The following methods are taken from Particle FW, specifically spark::StreamLogHandler.
// See https://github.com/spark/firmware/blob/develop/wiring/src/spark_wiring_logging.cpp
const char* MQTTManager::extractFileName(const char *s) {
    const char *s1 = strrchr(s, '/');
    if (s1) {
        return s1 + 1;
    }
    return s;
}

const char* MQTTManager::extractFuncName(const char *s, size_t *size) {
    const char *s1 = s;
    for (; *s; ++s) {
        if (*s == ' ') {
            s1 = s + 1;                                                                                                                         // Skip return type
        } else if (*s == '(') {
            break;                                                                                                                         // Skip argument types
        }
    }
    *size = s - s1;
    return s1;
}

// This method is how we are called by the LogManager
void MQTTManager::logMessage(const char *msg, LogLevel level, const char *category, const LogAttributes &attr) {
    String s;

//    LOG_LEVEL_ALL = 1
//    LOG_LEVEL_TRACE = 1
//    LOG_LEVEL_INFO = 30
//    LOG_LEVEL_WARN= 40
//    LOG_LEVEL_ERROR = 50
//    LOG_LEVEL_NONE = 70
    if (level < _logLevel) return;
    
    // Source file
    if (attr.has_file) {
        s = extractFileName(attr.file);                                                                                 // Strip directory path
        s.concat(s);                                                                                 // File name
        if (attr.has_line) {
            s.concat(":");
            s.concat(String(attr.line));                                                                                                                         // Line number
        }
        if (attr.has_function) {
            s.concat(", ");
        } else {
            s.concat(": ");
        }
    }

    // Function name
    if (attr.has_function) {
        size_t n = 0;
        s = extractFuncName(attr.function, &n);                                                                                 // Strip argument and return types
        s.concat(s);
        s.concat("(): ");
    }

    // Level
    s.concat(levelName(level));
    s.concat(": ");

    // Message
    if (msg) {
        s.concat(msg);
    }

    // Additional attributes
    if (attr.has_code || attr.has_details) {
        s.concat(" [");
        // Code
        if (attr.has_code) {
            s.concat(String::format("code = %p", (intptr_t)attr.code));
        }
        // Details
        if (attr.has_details) {
            if (attr.has_code) {
                s.concat(", ");
            }
            s.concat("details = ");
            s.concat(attr.details);
        }
        s.concat(']');
    }

    //TODO: If MQTT not connected, write to Serial instead
//    Serial.println(s);
    log(category, s);
}

// This is our formatter. We can format messages however we want.
void MQTTManager::log(const char *category, String message) {
    String time = Time.format(Time.now(), "%a %H:%M");

    if(!_logging) {
        _logging++;
        publish("patriot/log/"+_controllerName, time + " " + message);
        _logging--;
    }
}

