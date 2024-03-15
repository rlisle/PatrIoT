//---------------
// Handle Events
//---------------

// Called every minute to allow delayed turn-offs, etc.
void handleNextMinute() {
    if(isTimingOfficeDoor) {
        if(millis() > Device::msecsLastChange("OfficeDoor") + OFFICE_DOOR_TIMEOUT_MSECS) {
            Log.info("RP turning off isTimingOfficeDoor");
            isTimingOfficeDoor = false;
            updateLights();
        }
    }

    if(isTimingOfficeMotion) {
        if(Device::msecsLastChange("OfficeMotion") + OFFICE_MOTION_TIMEOUT_MSECS < millis()) {
            isTimingOfficeMotion = false;
            updateLights();
        }
    }
}

void handleOfficeDoor(int value, int oldValue) {
    Log.info("RP handleOfficeDoor %d", value);
    if(value > 0 && oldValue == 0) {        // Opened
        Log.info("RP door opened");
    } else if(value == 0 && oldValue > 0) { // Closed
        Log.info("RP door closed, setting isTimingOfficeDoor");
        isTimingOfficeDoor = true;
    }
}

void handleOfficeMotion(int value, int oldValue) {
    Log.info("RP handleOfficeMotion %d", value);
    if(value > 0 && oldValue == 0) {        // Movement
        isTimingOfficeMotion = true;
    } else if(value == 0 && oldValue > 0) { // No movement
        //nothing to do
    }
}

// MODES
// Setting some modes causes other modes to be cleared
// eg. Sleeping turns off Retiring
void handleNighttime(int value, int oldValue) {
    Log.info("handleNighttime %d", value);
    if(value > 0 && oldValue == 0) {
        //TODO:
    }
}
void handleRetiring(int value, int oldValue) {
    Log.info("handleRetiring %d", value);
    if(value > 0 && oldValue == 0) {
        set("Cleaning", 0);
        set("Desk", 0);
        set("Loft", 0);
        set("Office", 0);
        set("Outside", 0);
        set("Piano", 0);
        set("Sleeping", 0);
    }
}
void handleSleeping(int value, int oldValue) {
    Log.info("handleSleeping %d", value);
    if(value > 0 && oldValue == 0) {
        set("Cleaning", 0);
        set("Desk", 0);
        set("Loft", 0);
        set("Office", 0);
        set("Outside", 0);
        set("Piano", 0);
        set("Retiring", 0);
    }
}