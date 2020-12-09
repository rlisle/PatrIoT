/**
States collection

This object manages a collection of State objects.

Multiple states can be active at the same time, so it is
important to combine their effects, in addition to stopping a
state in a manner that doesn't break states that continue
to be in effect.

http://www.github.com/rlisle/Patriot

Written by Ron Lisle

BSD license, check LICENSE for more information.
All text above must be included in any redistribution.
*/

#include "states.h"
#include "constants.h"

/**
 * globalStatesVariable
 * Lists all the currently active states names in CSV format.
 */
String globalStatesVariable;

States::States() {
    _states = NULL;
    expose();
}

void States::expose() {
    globalStatesVariable = "";
    if (!Particle.variable(kStatesVariableName, globalStatesVariable)) {
        Log.error("Unable to expose " + String(kStatesVariableName) + " variable");
    }
}

// States are added only once
State *States::addState(String name, int value) {
    // Update existing state if it exists
    State *state = getStateWithName(name);
    if (state == NULL) {
        Log.info("States addState adding " + name + " = " + String(value));
        state = new State(name,value);
        if(_states == NULL) {
            _states = state;
        } else {
            State* ptr = _states;
            while(ptr->_next != NULL) ptr = ptr->_next;
            ptr->_next = state;
        }
    } else {    // State already exists
        Log.info("States addState updating " + name + " = " + String(value) + ", was " + String(state->_value));
        state->_value = value;
    }
    Log.info("addState state was added. Count = " + String(count()));
    buildStatesVariable();
    return state;
}

State *States::getStateWithName(String name) {
    State *ptr = _states;
    while(ptr != NULL) {
        if (ptr->_name.equalsIgnoreCase(name)) {
            return ptr;
        }
        ptr = ptr->_next;
    }
    Log.info("getStateWithName " + name + " not found");
    return NULL;
}

int States::count() {
    int i = 0;
    for(State* ptr = _states; ptr != NULL; ptr = ptr->_next) i++;
    return i;
}

void States::buildStatesVariable() {
    String newVariable = "";
    State *ptr = _states;
    while (ptr != NULL) {
        newVariable += ptr->_name;
        newVariable += ":";
        newVariable += String(ptr->_value);
        if (ptr->_next != NULL) {
            newVariable += ",";
        }
        ptr = ptr->_next;
    }
    if (newVariable.length() < kMaxVariableStringLength) {
        if (newVariable != globalStatesVariable) {
            globalStatesVariable = newVariable;
        }
    } else {
        Log.info("States variable is too long. Need to extend to a 2nd variable");
    }
}
