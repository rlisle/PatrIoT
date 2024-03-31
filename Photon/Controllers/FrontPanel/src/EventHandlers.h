//----------------
// Event Handlers
//----------------

// Called every minute to allow delayed turn-offs, etc.
void handleNextMinute() {
    if(isTimingFrontDoor) {
        if(Device::msecsLastChange("LivingRoomDoor") + frontDoorLightTimeout < millis()) {
            Log.info("Front door timed out");
            isTimingFrontDoor = false;
            updateLights();
        }
    }

    //TODO: move this to the device
    if(isTimingLivingRoomMotion) {
        if(millis() > Device::msecsLastChange("LivingRoomMotion") + livingRoomMotionTimeout) {            
            isTimingLivingRoomMotion = false;
            updateLights();
        }
    }

    if(isTimingCoffeeMotion) {
        if(millis() > Device::msecsLastChange("CoffeeMotion") + coffeeMotionTimeout) {            
            isTimingCoffeeMotion = false;
            updateLights();
        }
    }

}

void handleFrontDoor(int value, int oldValue) {
    if(value > 0 && oldValue == 0) {        // Opened
        updateLights();
    } else if(value == 0 && oldValue > 0) { // Closed
        isTimingFrontDoor = true;
        updateLights();
    }
}

// Called by livingRoomMotion
void wakeupIfAfter430am() {
    if(is("sleeping") == true 
        && Time.hour() < 10 
        && ((Time.hour() == 4 && Time.minute() >= 30)
        || (Time.hour() > 4))) {
            Log.info("FP wakeupIfAfter430am");
            set("sleeping", false);     // Is this needed?
            IoT::publishValue("sleeping/set", 0);
        }
}

void handleLivingRoomMotion(int value, int oldValue) {
    if(value > 0 && oldValue == 0) {        // Movement
        Log.info("FP LivingRoomMotion");
        isTimingLivingRoomMotion = true;
        wakeupIfAfter430am();
//        updateLights();     //TODO: unneeded now?
    } else if(value == 0 && oldValue > 0) { // No movement
//        updateLights();     //TODO: unneeded now/
    }
}

void handleCoffeeMotion(int value, int oldValue) {
    Log.info("FP handleCoffeeMotion %d", value);
    if(value > 0 && oldValue == 0) {        // Movement
        isTimingCoffeeMotion = true;
    } else if(value == 0 && oldValue > 0) { // No movement
        //nothing to do
    }
}

void handleRetiring(int value, int oldValue) {
    if(value > 0 && oldValue == 0) {        // Bedtime
        Log.info("FP handleRetiring");
        // Turn off other statuses
        set("Bedroom", 0);
        set("Cabinets", 0);
        set("Cleaning", 0);
        set("Kitchen", 0);
        set("Nook", 0);
        set("Outside", 0);
        set("Sink", 0);
        set("Sleeping", 0);
        set("Theatre", 0);
    }
}

void handleSleeping(int value, int oldValue) {
    if(value > 0 && oldValue == 0) {        // Sleeping
        Log.info("FP handleSleeping");
        // Turn off other statuses
        set("Bedroom", 0);
        set("Cabinets", 0);
        set("Cleaning", 0);
        set("Kitchen", 0);
        set("Nook", 0);
        set("Outside", 0);
        set("Retiring", 0);
        set("Sink", 0);
        set("Theatre", 0);
    }
}