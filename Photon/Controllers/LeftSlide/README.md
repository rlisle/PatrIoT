# LeftSlide

Controller mounted next to couch in the left slide.
It controls both light switches: Couch overhead and Left Vertical

It is accessible by removing the iPhone 6+

## Hardware
  New Photon 2 I/O Map
    A2/D13 LED1 PWM output
    A5/D14 LED2 PWM output
    S4/D19 PIR sensor
 
  Optional I/O
     2nd PIR S3/D18
 
  Previous I/O Map
    Tx HR24 sensor Rx
    Rx HR24 sensor Tx
    A0 PIR sensor
    A5 LED 2 PWM output
    A7 (WKP) LED1 PWM output
 
  Terminal Strip
    1 +3.3v (top)
    2 Gnd
    3 +12v
    4 -LED1
    5 -LED2

  Using SYSTEM_THREAD(ENABLED) is recommended,
  and runs network on separate theread.
  Using SYSTEM_MODE(SEMI_AUTOMATIC) we will
  manually connect, but everything is automatic
  after that. This allows running loop and MQTT
  even if no internet available
 
 History:
   8/6/23 Convert to Photon 2, remove Hue
   8/23/23 Embed plugins due to compiler issue

## Published Libraries
This project uses the published Patriot libraries:
* PatriotIoT (IoT)
* PatriotLight
* PatriotSwitch

When any library is updated, rerun "particle library add <name>" from the project directory.

## Code Organization

#### ```/src``` folder:  
Contains all the source files.
It should *not* be renamed.
Anything that is in this folder when you compile your project will be sent to our compile service
and compiled into a firmware binary for the Particle device that you have targeted.

If your application contains multiple files, they should all be included in the `src` folder.
If your firmware depends on Particle libraries, those dependencies are specified in
the `project.properties` file referenced below.

#### ```.ino``` file:
This file is the firmware that will run as the primary application on your Particle device.
It contains a `setup()` and `loop()` function, and can be written in Wiring or C/C++.
For more information about using the Particle firmware API to create firmware for your
Particle device, refer to the [Firmware Reference](https://docs.particle.io/reference/firmware/)
section of the Particle documentation.

#### ```project.properties``` file:  
This is the file that specifies the name and version number of the libraries that your project depends on.
Dependencies are added automatically to your `project.properties` file when you add a library to a
project using the `particle library add` command in the CLI or add a library in the Desktop IDE.

## Adding additional files to the project

#### Projects with multiple sources
If you would like add additional files to your application, they should be added to the `/src` folder.
All files in the `/src` folder will be sent to the Particle Cloud to produce a compiled binary.

#### Projects with external unpublished libraries
If your project includes a library that has not been registered in the Particle libraries system,
you should create a new folder named `/lib/<libraryname>/src` under `/<project dir>` and add
the `.h` and `.cpp` files for your library there. All contents of the `/lib` folder and subfolders
will also be sent to the Cloud for compilation.

## Compiling your project

  To update Photon:
    1. Edit this code
    2. Update IoT and plugins if needed
    3. "particle flash left_slide2 --target 5.6.0" or shortcut "fls"
