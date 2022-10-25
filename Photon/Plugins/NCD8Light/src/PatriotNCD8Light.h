/******************************************************************
 NCD Light control

 Features:
 - 0-100 dimming control
 - Supports multiple boards
 
 http://www.github.com/rlisle/Patriot

 Written by Ron Lisle

 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 Datasheets:

 ******************************************************************/

#pragma once

#include "Particle.h"
#include <device.h>

class NCD8Light : public Device
{
 private:
    int8_t  _lightNum;                 // Up to 8 lights supported
    int8_t   _address;                 // Address of board (eg. 0x40)

    int      _dimmingDuration;
    float    _currentLevel;            // Use for smooth dimming transitions.
    float    _targetLevel;
    float    _incrementPerMillisecond;
    
    long     _lastUpdateTime;

    int8_t  initializeBoard();
    void    outputPWM();
    void    startSmoothDimming();
    int     scalePWM(float value);

 public:
    NCD8Light(int8_t address, int8_t lightNum, String name, String room, int8_t duration = 0);
    void    begin();
    void    reset();
    void    setValue(int value);
    void    loop();
};
