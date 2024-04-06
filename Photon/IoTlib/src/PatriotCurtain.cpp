/**
 Patriot Curtain

 This plugin supports connection to an automated curtain.
 The curtain opens when pin 6 is briefly connected to pin 1 ground,
 and closes when pin 5 is briefly connect to pin 1 ground.
 Motion is stopped when the same pin is briefly connected again.

 This plugin is designed to coexist with up to 2 NCD4Relays
 and up to 4 NCD4Switch devices using the GPIOs on same board.
 
 Requires MCP23008 be loaded and running first.
 
 Features:
 - 2 relay automated curtain control
 - On (open), Off (closed), or timed partially open (1-99)
 - Supports multiple boards
 
 http://www.github.com/rlisle/Patriot

 Written by Ron Lisle
 Based on NCD8Relay library.

 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 Datasheets:

 Changelog:
 2023-09-11: Use mcp23008 and fixed 4 relay 4 GPIO
 2021-01-16: Initial creation based on NCD8Relay
 2022-04-17: Fix issue turning off other relays on same board.
 */

#include "IoT.h"

#define MILLIS_PER_SECOND 1000
#define FULL_TIME_MILLIS 6000   // Duration of full open to close or vis-vis
#define MILLIS_PER_PERCENT 60   // FULL_TIME_MILLIS / 100
//#define MILLIS_PER_UPDATE 250   // Send updates every 1/4 second
#define MILLIS_PER_UPDATE 1000  // Send updates every second
#define PULSE_MILLIS 100

//TODO: change to 1, 0 so -1 not needed elsewhere
// Mode (relay bit)
#define OPEN_CURTAIN 2
#define CLOSE_CURTAIN 1

/**
 * Constructor
 * @param boardAddress is the board address set by jumpers (0-7)
 * @param relayIndex is the relay number of the 1st of 2 relays (0-2)
 * @param name String name used to address the relay.
 */
Curtain::Curtain(int8_t relayIndex, String name, String room)
    : Device(name, room)
{
    _relayIndex  = relayIndex;      // 0 (first 2 relays)
    _stopMillis = 0;
    _mode = 0;
    _stage = 0;
    _type  = 'C';
    _holding = false;
}

void Curtain::begin() {
    // Handled by mcp23008
}

/**
 * Set value
 * This is how things are turned on/off in Patriot
 * @param percent Int 0 to 100. 0 = closed, >0 = open
 */
void Curtain::setValue(int percent) {
    if(percent == _value) {
        Log.warn("Curtain setValue is the same as previous value, ignoring");
        //TODO: do we want to issue open/close again just in case?
        return;
    }

    _startPosition = _value;
    _startMillis = millis();
    
    _value = percent;           // Should this report current instead?
    _holding = false;
    _updateMillis = millis() + MILLIS_PER_UPDATE;
    Log.info("_updateMillis = %ld",_updateMillis);

    // Send HomeKit acknowledgement
    IoT::publishMQTT("ack/" + _name + "/set",String(percent));
    
    // Send position updates
    IoT::publishMQTT(_name + "/get",String(percent));
    IoT::publishMQTT(_name + "/position",String(_startPosition));

    if(_value > _startPosition) {
        _mode = OPEN_CURTAIN;
        IoT::publishMQTT(_name + "/state", "increasing");

    } else {
        _mode = CLOSE_CURTAIN;
        IoT::publishMQTT(_name + "/state", "decreasing");
    }

    // We only need a single pulse if opening or closing all the way
    if(_value == 0 || _value == 100) {
        _stage = 3;
    } else {
        _stage = 1;
    }
    _stopMillis = millis() + PULSE_MILLIS;
    pulse(true);
}

void Curtain::setHold(bool holding) {
    if(holding == true) {
        if(_holding == true) {  // Already holding?
            return;
        }
        //TODO: stop movement, but remember target in case hold false
        
    } else {        // resume
        if(_holding == false) { // Not currently holding?
            return;
        }
        //TODO: resume movement - same as setValue using old target
        
    }
    _holding = holding;
    Log.warn("Curtain setHold not implemented");
}


/**
 * Start 1=close, 2=open
 */
void Curtain::pulse(bool high) {
    
    Log.info("Curtain pulse %s", high ? "high" : "low");
    int status = MCP23008::write(_relayIndex + _mode - 1, high);
    if(status != 0) {
        Log.error("Error pulsing relay %d %s", _relayIndex + _mode - 1, high ? "high" : "low");
    }
}

/**
 * loop()
 */
void Curtain::loop()
{
    if(isCurtainRunning()) {
        
        if(isTimeToChangePulse()) {
            switch(_stage) {
                case 1:
                    Log.info("Curtain end-of-start pulse");
                    pulse(false);
                    
                    _stopMillis = millis() + ((FULL_TIME_MILLIS *  abs(_startPosition - _value)) / 100) - PULSE_MILLIS;
                    
                    _stage = 2;
                    break;
                case 2:
                    Log.info("Curtain start-of-end pulse");
                    pulse(true);
                    _stopMillis = millis() + PULSE_MILLIS;
                    _stage = 3;
                    break;
                case 3:
                    Log.info("Curtain end-of-end pulse");
                    pulse(false);
                    _stopMillis = millis() + FULL_TIME_MILLIS - PULSE_MILLIS;
                    _stage = 4;
                    break;
                case 4:
                    _stage = 0;
                    IoT::publishMQTT(_name + "/state", "stopped");
                    break;
                default:
                    Log.error("Invalid _stage %d",_stage);
            }
            
        }
        
        // Calculate periodic HomeKit updates to getCurrentPosition
        if(millis() >= _updateMillis) {
            _updateMillis += MILLIS_PER_UPDATE;
            
            int percentDelta = (int)((millis() - _startMillis) / MILLIS_PER_PERCENT);
            if(_mode == CLOSE_CURTAIN) percentDelta = -percentDelta;
//            Log.info("DBG: _startPosition = %d",_startPosition);
//            Log.info("DBG: curtain percent delta = %d",percentDelta);
//            Log.info("DBG: _startMillis = %ld",_startMillis);
//            Log.info("DBG: millis = %ld",millis());
//            Log.info("DBG: millis delta = %ld",millis() - _startMillis);
//            _value = _startPosition + percentDelta;
//            IoT::publishMQTT(_name + "/position", String(_value));
            IoT::publishMQTT(_name + "/position", String(_startPosition + percentDelta));
        }
    }
};

bool Curtain::isCurtainRunning() {
    return(_stage != 0);
}

bool Curtain::isTimeToChangePulse() {
    return(millis() >= _stopMillis);
}
