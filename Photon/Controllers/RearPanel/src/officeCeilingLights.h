//------------
// officeCeilingLights
//------------
void setOfficeCeilingLights() {
    int percent = 0;
    int current = value("OfficeCeiling");

    if(is("Cleaning")) {
        percent = 100;

    } else if(is("Office")) {
        percent = valuem1("Office");

    } else switch(partOfDay()) {
        case Evening:
        case Retiring:
        case AwakeEarly:
        case Asleep:
        case Morning:
        case Afternoon:
            percent = 0;
    }
    if(percent != current) {
        set("OfficeCeiling", percent);
    }
}
