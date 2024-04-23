//------------
// PlantLight
//------------
void setPlantLight() {
    int percent = 0;
    int current = value("PlantLight");

    if(is("Cleaning")) {
        percent = 100;

    } else if(is("Kitchen")) {
        percent = valuem1("Kitchen");

    } else switch(partOfDay()) {
        case AwakeEarly:
        case Evening:
        case Retiring:
        case Asleep:
            break;
        case Morning:
        case Afternoon:
            percent = 100;
            break;
    }
    if(percent != current) {
        set("PlantLight", percent);
    }
}
