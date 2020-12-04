/******************************************************************
Patriot Activity plugin

Features:
- Allows Alexa to control a 'name' without hardware.
- This is how 'Activities' are implemented.

http://www.github.com/rlisle/Patriot

Written by Ron Lisle

BSD license, check license.txt for more information.
All text above must be included in any redistribution.

******************************************************************/
#pragma once

#include "Particle.h"
#include "device.h"

class Activity : public Device
{
private:
    
public:
    /**
     * Constructor
     */
    Activity(String name);
    
    void setPercent(int percent);
    void stateDidChange(States *states);
    
    bool shouldAutoCreateBehavior() { return false; };

};
