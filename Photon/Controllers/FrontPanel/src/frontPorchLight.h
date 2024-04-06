//------------
// FrontPorchLight
//------------
void setFrontPorchLight() {
    int percent = 0;
    int current = value("FrontPorch");

    if(is("Nighttime")) {
        if(is("Outside")) {
            percent = 100;
        } else if(is("FrontDoor") || isTimingFrontDoor) {
            percent = 100;
        } else switch(partOfDay()) {
            case Evening:
            case Retiring:
            case Asleep:
            case AwakeEarly:
            case Morning:
            case Afternoon:
                percent = 0;
        }
    }
    if(percent != current) {
        set("FrontPorch", percent);
    }
}
