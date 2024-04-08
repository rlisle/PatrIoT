//------------
// CabinetLamps
//------------
void setCabinetLamps() {
    int percent = 0;
    int current = value("CabinetLamps");

    if(is("Cleaning")) {
        percent = 100;

    } else if(is("Kitchen")) {
        percent = value("Kitchen");

    } else if(is("CoffeeMotion") || isTimingCoffeeMotion) {
        percent = 33;

    } else if(is("Theatre")) {
        percent = 0;

    } else switch(partOfDay()) {
        case Evening:
            percent = 100;
            break;
        case Retiring:
        case Asleep:
        case AwakeEarly:
        case Morning:
        case Afternoon:
            percent = 0;
    }
    if(percent != current) {
        set("CabinetLamps", percent);
    }
}
