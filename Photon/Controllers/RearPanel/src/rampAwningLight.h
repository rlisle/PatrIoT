//------------
// RampAwningLight
//------------
void setRampAwningLight() {
    int percent = 0;
    int current = value("RampAwning");

    if(is("Nighttime")) {
        Log.info("RAMP AWNING is Nighttime");
        if(is("Ramp")) {
            Log.info("RAMP AWNING Ramp = %d",valuem1("Ramp"));
            percent = valuem1("Ramp");
        } else if(is("Outside")){
            Log.info("RAMP AWNING Outside = %d",valuem1("Outside"));
            percent = valuem1("Outside");
        } else if(is("RampDoor") || isTimingRampDoor) {
            Log.info("RAMP AWNING RampDoor");
            percent = 100;
        } else switch(partOfDay()) {
            case Evening:
                Log.info("RAMP AWNING Evening");
                percent = 100;
                break;
            case Retiring:
            case Asleep:
            case AwakeEarly:
            case Morning:
            case Afternoon:
                percent = 0;
        }
    }
    if(percent != current) {
        Log.info("RAMP AWNING percent = %d",percent);
        set("RampAwning", percent);
    }
}
