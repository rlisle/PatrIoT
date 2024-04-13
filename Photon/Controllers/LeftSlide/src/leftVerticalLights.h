//----------------------
// LeftVerticalLights
//----------------------
void setLeftVerticalLights() {
    int percent = 0;
    int current = value("LeftVertical");

    if(is("Cleaning")) {
        percent = 100;

    } else if(is("Theatre")) {
        percent = 0;

    } else switch(partOfDay()) {
        case Evening:
            percent = 50;
            break;
        case Retiring:
        case Asleep:
        case AwakeEarly:
        case Morning:
        case Afternoon:
            percent = 0;
    }
    if(percent != current) {
        set("LeftVertical", percent);
    }
}