//------------
// Behaviors
//
// Relevant Status
//   AnyoneHome
//   Cleaning
//   Desk
//   Loft
//   Nighttime
//   Office
//   OfficeMotion
//   Outside
//   Piano
//   Retiring (bedtime)
//   RonHome
//   ShelleyHome
//   Sleeping
//------------
#define OFFICE_DOOR_LIGHT_TIMEOUT 15*1000

#include "loftLights.h"
#include "officeCeilingLights.h"

void setDesk(int value) {
    if(is("Cleaning")) {
        set("LeftDeskLamp", 100);
        set("RightDeskLamp", 100);
    } else {
        set("LeftDeskLamp", 0);
        set("RightDeskLamp", 0);
    }
}

void setPiano(int percent) {
    if(is("Cleaning")) {
        set("PianoSpot", 100);
    } else if(is("Piano")) {
        set("PianoSpot", value("Piano"));
    } else if(is("Office")) {
        set("PianoSpot", value("Office"));
    } else {
        set("PianoSpot", percent);
    }
}

// Outside Overrides
void setRearPorch(int value) {
    if(is("Nighttime")) {
        if(is("Outside") || is("OfficeDoor") || isTimingOfficeDoor) {
            Log.info("RP setRearPorch 100");
            set("RearPorch", 100);
        } else {
            Log.info("RP setRearPorch %d", value);
            set("RearPorch", value);
        }
    } else {
        set("RearPorch", 0);    // Turn off if daytime
    }
}

void setRearAwning(int value) {
    if(is("Nighttime")) {
        if(is("Outside") || is("OfficeDoor") || isTimingOfficeDoor) {
            set("RearAwning", 100);
        } else {
            set("RearAwning", value);
        }
    } else {
        set("RearAwning", 0);    // Turn off if daytime
    }
}

void setRampPorch(int value) {
    if(is("Nighttime")) {
        if(is("Outside")) {
            set("RampPorch", 100);
        } else {
            set("RampPorch", value);
        }
    } else {
        set("RampPorch", 0);    // Turn off if daytime
    }
}

void setRampAwning(int value) {
    if(is("Nighttime")) {
        if(is("Outside")) {
            set("RampAwning", 100);
        } else {
            set("RampAwning", value);
        }
    } else {
        set("RampAwning", 0);    // Turn off if daytime
    }
}

// Update all devices managed by this Photon2
void updateLights() {

    setLoftLights();
    setOfficeCeilingLights();

    switch(partOfDay()) {
        case Asleep:
            Log.info("RP updateLights Asleep");
            set("Curtain", 0);     // Close curtain

            setDesk(0);
            setPiano(0);

            setRampPorch(0);
            setRampAwning(0);
            setRearAwning(0);
            setRearPorch(0);
            break;

        case AwakeEarly:
            Log.info("RP updateLights AwakeEarly");
            setPiano(5);
            setDesk(100);

            setRampPorch(0);
            setRampAwning(0);
            setRearAwning(0);
            setRearPorch(0);
            break;

        case Morning:
        case Afternoon:
            Log.info("RP updateLights daytime");
            setPiano(0);
            setDesk(100);

            setRampPorch(0);
            setRampAwning(0);
            setRearAwning(0);
            setRearPorch(0);
            break;

        case Evening:
            Log.info("RP updateLights evening");
            setPiano(50);
            setDesk(100);

            setRampPorch(100);
            setRampAwning(100);
            setRearAwning(100);
            setRearPorch(100);
            break;

        case Retiring:
            Log.info("RP updateLights retiring");

            set("Curtain", 0);     // Close curtain
            
            setPiano(5);
            setDesk(33);

            setRampPorch(0);
            setRampAwning(0);
            setRearAwning(0);
            setRearPorch(0);
            break;
    }
}
