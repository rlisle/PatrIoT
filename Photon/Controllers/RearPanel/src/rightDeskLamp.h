//------------
// RightDeskLamp
//------------
void setRightDeskLamp() {
    int percent = 0;
    int current = value("RightDeskLamp");
    Log.info("Current RightDeskLamp %d",current);

    if(is("Cleaning")) {
        percent = 100;

    } else if(is("Desk")) {
        percent = valuem1("Desk");
        Log.info("Desk override RightDeskLamp %d",percent);

    } else if(is("Office")) {
        percent = valuem1("Office");

    } else switch(partOfDay()) {
        case AwakeEarly:
        case Morning:
        case Afternoon:
        case Evening:
            // if(is("RonHome")) {
                 percent = 100;
            // } else {
            //   Log.info("Not turning on RightDeskLamp because Ron isn't home");
            // }
            break;
        case Retiring:
            percent = 10;
            break;
        case Asleep:
            break;
    }
//    if(percent != current) {
        Log.info("Setting RightDeskLamp %d",percent);
        set("RightDeskLamp", percent);
//    }
}
