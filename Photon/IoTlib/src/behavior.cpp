/**
Behavior

This class represents a behavior, which is a response to one or
more states such as "tv" or "night".

http://www.github.com/rlisle/Patriot

Written by Ron Lisle

BSD license, check license.txt for more information.
All text above must be included in any redistribution.

*/
#include "behavior.h"

Behavior::Behavior(int level, bool isDefault)
{
    _isDefault = isDefault;
    _level = level;
    _conditions = new Conditions();
    _next = NULL;
}

void Behavior::addCondition(Condition *condition) {
    _conditions->addCondition(condition);
}

int Behavior::evaluateStates(States *states) 
{
    //Serial.println("Evaluating states: ("+String(_conditions->count())+" conditions)");
    for(int x=0; x<_conditions->count(); x++){
        Condition* condition = _conditions->getCondition(x);
        if(condition->isTrue(states) == false) {
            return 0;
        }
        // If true, and isDefault, then set _level to state's level
        if (_isDefault) {
            _level = condition->getStateValue(states);
        }
    }
    return _level;
}
