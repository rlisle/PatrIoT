//------------
// Behaviors
//
// Relevant Status
//   AnyoneHome
//   Bedtime
//   Cleaning
//   Kitchen
//   LivingRoomMotion
//   Nighttime
//   Outside
//   RonHome
//   ShelleyHome
//   Sink
//   Sleeping
//------------
#include "bedroomLamp.h"
#include "cabinetLamps.h"
#include "ceilingLights.h"
#include "doorSideLights.h"
#include "frontAwningLight.h"
#include "frontPorchLight.h"
#include "kitchenCeilingLamps.h"
#include "leftTrimLights.h"
#include "nookLamp.h"
#include "otherSideLights.h"
#include "plantLight.h"
#include "rightTrimLights.h"
#include "sinkLamp.h"

// Update all devices managed by this Photon2
void updateLights() {

    // When cleaning is set, all inside lights are turned on
    // Turn on all outside lights also if it is nighttime
    // Assuming that not bedtime or sleeping when cleaning
    if(is("Cleaning")) {
        // Turn off other statuses
        set("Retiring", 0);
        set("Sleeping", 0);
        return;
    }

    switch(partOfDay()) {
        case Retiring:
        case Asleep:
        case AwakeEarly:
        case Morning:
        case Afternoon:
        case Evening:
            // Nothing to do
            break;
    }

    // Call individual device behavior files
    setBedroomLamp();
    setCabinetLamps();
    setCeilingLights();
    setDoorSideLights();
    setFrontAwningLight();
    setFrontPorchLight();
    setKitchenCeilingLamps();
    setLeftTrimLights();
    setNookLamp();
    setOtherSideLights();
    setPlantLight();
    setRightTrimLights();
    setSinkLamp();
}

