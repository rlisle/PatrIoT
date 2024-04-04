//------------
// PianoLight
//------------
void setPianoLight() {
    int percent = 0;
    int current = value("PianoSpot");

    if(is("Cleaning")) {
        percent = 100;

    } else if(is("Piano")) {
        percent = valuem1("Piano");

    } else if(is("Office")) {
        percent = valuem1("Office");

    } else switch(partOfDay()) {
        case Evening:
        case Retiring:
        case Asleep:
        case AwakeEarly:
        case Morning:
        case Afternoon:
            percent = 0;
    }
    if(percent != current) {
        set("PianoSpot", percent);
    }
}
