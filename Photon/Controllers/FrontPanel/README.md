# Front Panel

Controller mounted behind the front control panel.
It controls all switches on the front panel.

It is accessible by removing the entire wood panel.

## Compiling the project

To test compile, CD to FrontPanel then run "particle compile photon2"

To update Photon:
   1. Edit the code in FrontPanel.ino
   2. Update IoT and plugins if needed

   USB:
   3. Put Photon into listen mode using buttons
   4. "particle flash --usb <binname>"

   OTA:
   3. "particle flash front_panel2 --target 5.6.0" or "ffp"

## Hardware
NCD 16x Dimmer
  1. DS Flood Lights
  2. Awning Light
  3. Ceiling
  4. Porch Lights
  5. ODS Flood Lights
  6.
  7.
  8.
  9.
  10.
  11.
  12. Indirect
  13. Cabinet
  14. Sink
  15. Ceiling
  16. ?
 
  PHOTON 2 PINS (in order on card)
  Left side
  RST - nc
  3V3 - nc
  Mode - nc
  Gnd - nc              - 12v monitor R ladder gnd
  D11/A0/ADC4           - 12v monitor R ladder
  D12/A1/ADC5
  D13/A2/ADC3/PWM
  D14/A5/ADC0/PWM
  D19/S4                - Front Door switch
  D18/S3
  D17/SCK
  D15/MOSI/PWM          - Ceiling LEDs
  D16/MISO/PWM          - Cabinets LEDs
  D9/RX
  D8/TX
 
  Right Side
  LI+ 
  EN 
  VSUB (5v)
  D10
  D7
  D6
  D5
  D4
  D3
  D2
  D1/A4/ADC1/PWM/Wire SCL   - NCD I2C
  D0/A3/ADC2/Wire SDA       - NCD I2C
 
  SWITCH WIRING
  Top left:      tape label "Cabinet" -> D3 green -> gold
  Top right:     no label (right trim) #4 yellow
  2nd left:      tape label "Sink" #2 white
  2nd middle:    tape label "Ceiling" (kitchen) #1 red
  2nd right:     tape label "Indirect" (left trim) #5 blue
  gnd            brown?

  3rd left:      "Ceiling" D2 blue -> silver
  3rd l-m:       "DS Flood Lights" #0 green
  3rd r-m:       "ODS Flood Lights" #3 yellow
  3rd right:     "Porch Lights" #7 red
  bottom:        "Light" (awning) #6 white

  TERMINAL STRIPS
   Top: LED Drivers
    *
    *
    *
    *
    *
    *
   Top 12v: Fuse #5 Purple/White
   Gnd
   Middle 12v: Fuse #2 Orange/White
            also Fuse ? Purple/White marked Ceiling Power
   Bottom 12v: Fuse #14 Gray/White Awning
 
 Fuse Panel
 1. Brown/White     Bedroom Lts.
 2. Orange/White    Puck Lts (Ceiling Power)
 3. Green/White     Storage Lts
 4. Blue/White      Br Slide out
 5. Purple/White    Kitchen (Front Panel)
 6. Pink solid      Furnace
 7. Yellow/White    Fan
 8. Pink/White      Fuel Pump
 9. Blue solid      Water Pump
 10. Black/White    Monitor Panel
 11. Green/White    Ent
 12. n/c
 13. Red/White      Garage (RearPanel)
 14. Gray/White     Awning
 15. New Red        Pepwave
 16. n/c
 17. n/c
 18. n/c
 
  Photon 2 is 0a10aced202194944a0446ac front_panel2
  RSSI = -43dBm on 2/19/24
 
  Original Photon was 430033000f47343339383037 FrontPanel
  RSSI = -66dBm  on 10/1/22
 
 History
 02/19/24 Upgrade to new architecture
 10/13/23 Upgrade NCD 8x dimmer with 16x board (12-bit)## Published Libraries
This project uses the published Patriot libraries:
* PatriotIoT (IoT)
* PatriotLight
* PatriotMotorized

When any library is updated, rerun "particle library add <name>" from the project directory.
