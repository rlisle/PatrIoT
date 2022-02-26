/******************************************************************
PatriotMR24 plugin

Features:
- Use Rx/Tx if pins specified as 0,0

http://www.github.com/rlisle/Patriot

This is a Patriot plugin. After making changes use "particle library upload", etc.
 
Written by Ron Lisle

BSD license, check license.txt for more information.
All text above must be included in any redistribution.

******************************************************************/

#include "PatriotMR24.h"
#include "IoT.h"

// All this stuff is in case we want to use Rx/Tx instead of S1, S2
#define POLL_INTERVAL_MILLIS 500
#define MESSAGE_HEAD 0x55
#define ACTIVE_REPORT 0x04
#define FALL_REPORT 0x06

#define REPORT_RADAR 0x03
#define REPORT_OTHER 0x05

#define HEARTBEAT 0x01
#define ABNORMAL 0x02
#define ENVIRONMENT 0x05
#define BODYSIGN 0x06
#define CLOSE_AWAY 0x07

#define CA_BE 0x01
#define CA_CLOSE 0x02
#define CA_AWAY 0x03
#define SOMEBODY_BE 0x01
#define SOMEBODY_MOVE 0x01
#define SOMEBODY_STOP 0x00
#define NOBODY 0x00

// Return values
#define DETECTED_NOBODY 0
#define DETECTED_SOMEBODY_FAR 25
#define DETECTED_SOMEBODY 50
#define DETECTED_SOMEBODY_CLOSE 75
#define DETECTED_MOVEMENT 100

/**
 * Constructor
 * @param pinNum int pin number that is connected to the sensor output
 * @param name  String name of the event to send when sensor changes
 */
MR24::MR24(int s1pin, int s2pin, String name, String room)
        : Device(name, room)
{
    _type  = 'M';
    _value = 0;
    _s1value = 0;
    _s2value = 0;
    _s1pin = s1pin;
    _s2pin = s2pin;
    _index = 0;
}

void MR24::begin() {
    _lastPollTime = millis();
    if (usingS1S2()) {
        pinMode(_s1pin, INPUT_PULLDOWN);
        pinMode(_s2pin, INPUT_PULLDOWN);
    } else {
        Serial1.begin(9600);    // 9600 baud, default 8 bits, no parity, 1 stop bit SERIAL_8N1
    }
}

/**
 * loop()
 * This is called repeatedly to allow device to do its work.
 */
void MR24::loop()
{
    if (isTimeToCheckSensor())
    {
        if (didSensorChange())
        {
            notify();
        }
    }
}


// Private Helper Methods
bool MR24::usingS1S2() {
    return (_s1pin > 0 && _s2pin > 0);
}

/**
 * isTimeToCheckSensor()
 * @return bool if enough time has elapsed to sample switch again
 */
bool MR24::isTimeToCheckSensor()
{
    if(!usingS1S2()) return true;
    
    long currentTime = millis();
    if (currentTime < _lastPollTime + POLL_INTERVAL_MILLIS)
    {
        return false;
    }
    _lastPollTime = currentTime;
    return true;
}


/**
 * didSensorChange()
 *
 * @return bool if switch has changed since last reading
 */
bool MR24::didSensorChange() {
    if(usingS1S2()) {
        return didS1S2sensorChange();
    } else {
        return didTxRxSensorChange();
    }
}

bool MR24::didS1S2sensorChange()
{
    int oldS1 = _s1value;
    int oldS2 = _s2value;
    _s1value = digitalRead(_s1pin);
    _s2value = digitalRead(_s2pin);
    _value = _s1value ? 25 : 0;
    _value += _s2value ? 50 : 0;
    return (oldS1 != _s1value || oldS2 != _s2value);
}

bool MR24::didTxRxSensorChange()
{
    int Msg;
    int prevValue = _value;

    while(Serial1.available()) {
        Msg = Serial1.read();
        if(Msg == MESSAGE_HEAD && _index > 0){  // Skip first time
            _index = 0;             // Loop back to start of buffer
            _value = situation_judgment(_data[5], _data[6], _data[7], _data[8], _data[9]);
            logMessage();
        }
        if(_index < 14) {
            _data[_index++] = Msg;
        }
    }
    return _value != prevValue;
}

//    Msg = Serial1.read();
//    if(Msg == MESSAGE_HEAD){
//        delay(25);
//        length = Serial1.read();
//        delay(25);
//        length += Serial1.read() << 8;
//        delay(25);
//        function = Serial1.read();
//        delay(25);
//        address1 = Serial1.read();
//        delay(25);
//        address2 = Serial1.read();
//        delay(25);
//        for(i=0; i<length-7; i++) {
//            data[i] = Serial1.read();
//            delay(25);
//        }
//        chksum = Serial1.read();
//        delay(25);
//        chksum += Serial1.read() << 8;
//
//        newValue = situation_judgment(data[0], data[1], data[2], data[3], data[4]);
//
//        if(function == 4) {
//            String proactive = "Proactive ";
//            if(address1 == 1) {
//                proactive += "module ID ";  // address2 s/b 2
//            }
//            if(address1 == 3){
//                if(address2 == 5) {
//                    if(data[0]==0 && data[1]==255 && data[2] == 255) proactive += "radar unoccupied ";
//                    if(data[0]==1 && data[1]==0 && data[2] == 255) proactive += "radar stationary ";
//                    if(data[0]==1 && data[1]==1 && data[2] == 1) proactive += "radar people moving ";
//                }
//                if(address2 == 6) {
//                    proactive += "radar motor (float) ";
//                }
//                if(address2 == 7) {
//                    if(data[0]==1 && data[1]==1 && data[2] == 1) proactive += "radar approaching away none ";
//                    if(data[0]==1 && data[1]==1 && data[2] == 2) proactive += "radar approaching away close ";
//                    if(data[0]==1 && data[1]==1 && data[2] == 3) proactive += "radar approaching stay away ";
//                }
//            }
//            if(address1 == 5) {
//                if(address2 == 1) {
//                    if(data[0]==0 && data[1]==255 && data[2] == 255) proactive += "radar Heartbeat unoccupied ";
//                    if(data[0]==1 && data[1]==0 && data[2] == 255) proactive += "radar Heartbeat stationary ";
//                    if(data[0]==1 && data[1]==1 && data[2] == 1) proactive += "radar Heartbeat people moving ";
//                }
//            }
//            IoT::mqttPublish("DEBUG1:",proactive);
//
//        } else {
//            String status = String::format("Status %d, len=%d, func=%d, a1=%d, a2=%d data: %d, %d, %d, %d, %d chksum=%x",newValue,length,function,address1,address2,data[0],data[1],data[2], data[3], data[4],chksum);
//            IoT::mqttPublish("DEBUG2:",status);
//        }
//}

void MR24::logMessage() {
    int length = 0;
    int function = 0;
    int address1 = 0;
    int address2 = 0;

    length = _data[1] + (_data[2] << 8);
    function = _data[3];
    address1 = _data[4];
    address2 = _data[5];
    // Data in data[6-10]
    //TODO: account for length
    //TODO: get and check CRC

    if(function == 4) {
        String proactive = "Proactive ";
        if(address1 == 1) {
            proactive += "module ID ";  // address2 s/b 2
        }
        if(address1 == 3){
            if(address2 == 5) {
                if(_data[6]==0 && _data[7]==255 && _data[8] == 255) proactive += "radar unoccupied ";
                if(_data[6]==1 && _data[7]==0 && _data[8] == 255) proactive += "radar stationary ";
                if(_data[6]==1 && _data[7]==1 && _data[8] == 1) proactive += "radar people moving ";
            }
            if(address2 == 6) {
                proactive += "radar motor (float) ";
            }
            if(address2 == 7) {
                if(_data[6]==1 && _data[7]==1 && _data[8] == 1) proactive += "radar approaching away none ";
                if(_data[6]==1 && _data[7]==1 && _data[8] == 2) proactive += "radar approaching away close ";
                if(_data[6]==1 && _data[7]==1 && _data[8] == 3) proactive += "radar approaching stay away ";
            }
        }
        if(address1 == 5) {
            if(address2 == 1) {
                if(_data[6]==0 && _data[7]==255 && _data[8] == 255) proactive += "radar Heartbeat unoccupied ";
                if(_data[6]==1 && _data[7]==0 && _data[8] == 255) proactive += "radar Heartbeat stationary ";
                if(_data[6]==1 && _data[7]==1 && _data[8] == 1) proactive += "radar Heartbeat people moving ";
            }
        }
        Log.info(proactive);

    } else {
        String status = String::format("Status %d, len=%d, func=%d, a1=%d, a2=%d data: %d, %d, %d, %d, %d ", _value,length,function,address1,address2,_data[6],_data[7],_data[8], _data[9], _data[10]);
        Log.info(status);
    }
}

int MR24::situation_judgment(int ad1, int ad2, int ad3, int ad4, int ad5)
{
    if(ad1 == REPORT_RADAR || ad1 == REPORT_OTHER){ // 0x03, 0x05
        if(ad2 == ENVIRONMENT || ad2 == HEARTBEAT){ // 0x05, 0x01
            if(ad3 == NOBODY){                      // 0x00
                return DETECTED_NOBODY;             // 0
            }
            else if(ad3 == SOMEBODY_BE && ad4 == SOMEBODY_MOVE){
                Log.info("radar said somebody move");
                return DETECTED_MOVEMENT;
            }
            else if(ad3 == SOMEBODY_BE && ad4 == SOMEBODY_STOP){
                Log.info("radar said somebody stop");
                return DETECTED_SOMEBODY;
            }
        }
        else if(ad2 == CLOSE_AWAY){
            if(ad3 == CA_BE && ad4 == CA_BE){
                if(ad5 == CA_BE){
                    Log.info("radar said no move");
                    return DETECTED_SOMEBODY;
                }
                else if(ad5 == CA_CLOSE){
                    Log.info("radar said somebody close");
                    return DETECTED_SOMEBODY_CLOSE;
                }
                else if(ad5 == CA_AWAY){
                    Log.info("radar said somebody away");
                    return DETECTED_SOMEBODY_FAR;
                }
            }
        }
    }
    return DETECTED_NOBODY;
}


/**
 * notify()
 * Publish switch state
 */
void MR24::notify()
{
    String topic = "patriot/" + _name;
    String message = String(_value);
    IoT::mqttPublish(topic,message);
}
