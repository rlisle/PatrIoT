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

#include "curtain.h"
#include "leftDeskLamp.h"
#include "loftLights.h"
#include "officeCeilingLights.h"
#include "officeValence.h"
#include "pianoLight.h"
#include "rampAwningLight.h"
#include "rampPorchLight.h"
#include "rearAwningLight.h"
#include "rearPorchLight.h"
#include "rightDeskLamp.h"

// Update all devices managed by this Photon2
void updateLights() {
    Log.info("RP anyChangeHandler");
    setCurtain();
    setLeftDeskLamp();
    setLoftLights();
    setOfficeCeilingLights();
    setOfficeValence();
    setPianoLight();
    setRampAwningLight();
    setRampPorchLight();
    setRearAwningLight();
    setRearPorchLight();
    setRightDeskLamp();
}

void setInitialState() {
    //TODO: move these to IoT?
    set("RonHome", 100);
    set("ShelleyHome", 100);
    set("AnyoneHome", 100);
    //TODO: rough estimate if Nighttime
    if(Time.hour() < 6 || Time.hour() > 6) {
        set("Nighttime", 100);
    }
}
