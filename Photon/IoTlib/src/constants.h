/**
Constants file

This file contains common constants used by the Patriot IoT library.

http://www.github.com/rlisle/Patriot

Written by Ron Lisle

BSD license, check LICENSE for more information.
All text above must be included in any redistribution.

*/
#pragma once

// EEPROM addresses (2047 bytes total)
#define VERSION_ADDR 0      // 1 byte 0 - 254, 255 = uninitialized
#define TIMEZONE_ADDR 1     // 1 byte signed, -127 to +128 offset from GMT
#define LATITUDE_ADDR 4     // 4 byte float
#define LONGITUDE_ADDR 8    // 4 byte float

static const int    kMaxVariableStringLength   = 864; // was 622 < v3.0.0
static const String kDevicesVariableName       = "Devices";
static const String kChecklistVariableName     = "Checklist";
static const String kStatusVariableName        = "Status";
static const String kOnStatusVariableName      = "OnStatus";
static const String kOffStatusVariableName     = "OffStatus";
static const String kPublishName               = "patriot";
static const String kSSIDFunctionName          = "setSSID";
static const String kTimeZoneFunctionName      = "setTimezone";

