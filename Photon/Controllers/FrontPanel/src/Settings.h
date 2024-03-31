// FrontPanel Settings
// Move the stuff that never changes here
#define CONTROLLER_NAME "FrontPanel2"
#define MQTT_BROKER "192.168.0.33"
#define PCA9685_ADDRESS 0x41       // Lowest jumper set
//#define SWITCH_ADDRESS 0x20
//#define SWITCH_IOMAP 0xFF       // All 8 GPIOs are inputs

int const frontDoorLightTimeout = 15*1000;
int const livingRoomMotionTimeout = 3*1000;
int const coffeeMotionTimeout = 3*1000;

#define CURVE 2 // 0 = Linear, 1 = exponential, 2 = 50/50

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(AUTOMATIC);

// Write Log messages to MQTT and/or serial port
#define MQTT_LOGGING true
//SerialLogHandler logHandler1(57600, LOG_LEVEL_INFO);

// Global Variables
int voltage = 0;
bool isTimingLivingRoomMotion = false;
bool isTimingFrontDoor = false;
bool isTimingCoffeeMotion = false;

void setupWifi() {
//    WiFi.setCredentials(WIFI_SSID, WIFI_PASSWORD);
//    WiFi.selectAntenna(ANT_INTERNAL);
//    WiFi.useDynamicIP();
}
