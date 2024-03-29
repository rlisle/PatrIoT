//------------
// officeValence power switch
//------------
void setOfficeValence() {
    int percent = 0;
    int current = value("OfficeValence");

    if(is("Cleaning")) {
        percent = 100;

    } else if(is("Office")) {
        percent = valuem1("Office");

    } else switch(partOfDay()) {
        case Evening:
            // if(is("RonHome")) {
                percent = 100;
            // } else {
            //    Log.info("Not turning on OfficeCeiling because Ron isn't home");
            // }
            break;
            
        case Retiring:
        case AwakeEarly:
        case Asleep:
        case Morning:
        case Afternoon:
            percent = 0;
    }
    if(percent != current) {
        set("OfficeValence", percent);
    }
}
